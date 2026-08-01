"""Offline RAG index construction.

Uses tree-sitter C parser to extract:
  1. Function signatures (return type, params, doc comment)
  2. Type definitions (struct, enum, union)
  3. Call patterns (each call_expression + surrounding code)
  4. Module-level code chunks (coarse-grained file sections)

Usage:
    from rag.build_index import build_index, index_exists
    build_index(source_dir, db_path, index_dir)
"""
import os
import re
from pathlib import Path

from rag.index_store import IndexStore
from rag.embedder import Embedder
from rag.config import EMBED_BATCH_SIZE

_C_LANG = None
_PARSER = None

_INIT_PATTERNS = re.compile(
    r'(?:^|_)(?:init|create|open|start|setup|begin|alloc|new|register|'
    r'config|enable|disable|reset|destroy|close|stop|cleanup|free|delete|'
    r'write|read|send|recv|get|set|handle|process|check|verify)',
    re.IGNORECASE,
)

_SKIP_DIRS = {
    ".git", "__pycache__", "bin", "build", "dist", "doc", "docs",
    "examples", "tests", "unittests", "test", "fuzzing", "patches",
    "riot-claude", "qsem-claude", "trajectory", "results",
    "rag_index", ".claude",
}

_NOISE_PAT = re.compile(
    r'\b(?:const|static|inline|extern|volatile|register|restrict|'
    r'unsigned|signed)\b'
)


def _find_grammar_so():
    """Find the tree-sitter C grammar shared library."""
    import tree_sitter_c
    # Try the newer tree-sitter-c package path
    pkg_dir = Path(tree_sitter_c.__file__).parent
    for pattern in ["*.so", "*.dll", "*.dylib", "**/*.so"]:
        for f in pkg_dir.glob(pattern):
            if f.is_file():
                return str(f)
    raise FileNotFoundError("Cannot find tree-sitter C grammar .so/.dll")


def _get_parser():
    """Lazily initialize tree-sitter C parser, supporting dual API versions."""
    global _C_LANG, _PARSER
    if _PARSER is not None:
        return _PARSER

    try:
        # tree-sitter >= 0.24: Language(lang) constructor
        import tree_sitter_c
        from tree_sitter import Language, Parser
        _C_LANG = Language(tree_sitter_c.language())
        _PARSER = Parser(_C_LANG)
    except Exception:
        # tree-sitter < 0.24: Language(path, name)
        from tree_sitter import Language, Parser
        so_path = _find_grammar_so()
        _C_LANG = Language(so_path, "c")
        _PARSER = Parser()
        _PARSER.set_language(_C_LANG)

    return _PARSER


def _node_text(node, source_bytes):
    """Get the source text of a syntax node."""
    return source_bytes[node.start_byte:node.end_byte].decode("utf-8", errors="replace")


def _clean_noise(text):
    """Remove storage-class specifiers to normalize signature text."""
    return _NOISE_PAT.sub("", text)


def iter_source_files(source_dir):
    """Walk source_dir yielding (rel_path, abs_path) for .c and .h files."""
    source_dir = Path(source_dir)
    for root, dirs, files in os.walk(source_dir):
        # Filter out skip dirs in-place
        dirs[:] = [d for d in dirs if d not in _SKIP_DIRS
                   and not d.startswith(".")]
        for fname in files:
            if fname.endswith((".c", ".h")):
                abs_path = Path(root) / fname
                rel_path = abs_path.relative_to(source_dir)
                yield str(rel_path), abs_path


def index_exists(db_path, index_dir):
    """Check if the RAG index has already been built."""
    db_path = Path(db_path)
    index_dir = Path(index_dir)
    return (db_path.exists()
            and (index_dir / "call_patterns.faiss").exists()
            and (index_dir / "module_code.faiss").exists())


# ============================================================
# Extraction functions
# ============================================================

def _extract_signatures(root_node, source_bytes, rel_path):
    """Extract function definitions from a C file."""
    results = []
    for node in root_node.children:
        if node.type != "function_definition":
            continue

        # Get function declarator
        decl = node.child_by_field_name("declarator")
        if decl is None:
            continue

        # Function name
        func_name = None
        for child in decl.children:
            if child.type == "function_declarator":
                name_node = child.child_by_field_name("declarator")
                if name_node is not None:
                    func_name = _node_text(name_node, source_bytes)
                break
            elif child.type == "identifier":
                func_name = _node_text(child, source_bytes)
                break
            elif child.type == "field_identifier":
                func_name = _node_text(child, source_bytes)
                break

        if func_name is None:
            continue

        # Return type
        ret_type_node = node.child_by_field_name("type")
        return_type = (
            _clean_noise(_node_text(ret_type_node, source_bytes)).strip()
            if ret_type_node else ""
        )

        # Parameters
        params = ""
        param_node = decl.child_by_field_name("parameters")
        if param_node is not None:
            params = _node_text(param_node, source_bytes)

        # Doc comment: find preceding comment node
        doc = _find_doc_comment(node, source_bytes)

        # Full signature
        sig_start = ret_type_node.start_byte if ret_type_node else node.start_byte
        sig_end = decl.end_byte
        signature = _clean_noise(
            source_bytes[sig_start:sig_end].decode("utf-8", errors="replace")
        ).strip()

        results.append({
            "func_name": func_name,
            "file_path": rel_path,
            "signature": signature,
            "return_type": return_type,
            "params": params,
            "doc_comment": doc,
        })

    return results


def _find_doc_comment(func_node, source_bytes):
    """Find the doc comment (/** ... */ or ///) preceding a function definition."""
    start = func_node.start_byte
    # Search backwards for comment
    search_start = max(0, start - 2000)
    prefix = source_bytes[search_start:start].decode("utf-8", errors="replace")

    # Look for /** ... */ style
    m = re.search(r'/\*\*?\s*\n?\s*\*?\s*(.+?)\s*\*/\s*$', prefix, re.DOTALL)
    if m:
        doc = m.group(1).strip()
        # Clean up leading * on each line
        doc = re.sub(r'\n\s*\*\s?', ' ', doc)
        return doc[:500]

    # Look for /// style
    lines = prefix.split("\n")
    doc_lines = []
    for line in reversed(lines):
        stripped = line.strip()
        if stripped.startswith("///"):
            doc_lines.insert(0, stripped.lstrip("/ ").strip())
        elif stripped.startswith("//") and doc_lines:
            doc_lines.insert(0, stripped.lstrip("/ ").strip())
        elif doc_lines:
            break
    return " ".join(doc_lines)[:500] if doc_lines else ""


def _extract_types(root_node, source_bytes, rel_path):
    """Extract struct, enum, union type definitions."""
    results = []
    for node in root_node.children:
        if node.type not in ("struct_specifier", "enum_specifier", "union_specifier"):
            continue

        kind = node.type.replace("_specifier", "")

        # Type name
        name_node = node.child_by_field_name("name")
        if name_node is None:
            # Check for typedef: struct { ... } name;
            name_node = node.child_by_field_name("declarator")
        type_name = _node_text(name_node, source_bytes) if name_node else "<anonymous>"

        # Full definition text
        definition = _node_text(node, source_bytes)

        # Skip very short definitions (forward declarations)
        if len(definition.strip()) < 15:
            continue

        results.append({
            "type_name": type_name,
            "kind": kind,
            "file_path": rel_path,
            "definition": definition,
        })

    return results


def _extract_call_patterns(root_node, source_bytes, rel_path):
    """Extract call expressions with surrounding context."""
    results = []
    lines = source_bytes.decode("utf-8", errors="replace").split("\n")

    def _walk(node):
        if node.type == "call_expression":
            func_node = node.child_by_field_name("function")
            if func_node is not None:
                called_func = _node_text(func_node, source_bytes)
                # Only collect non-trivial calls
                if (len(called_func) > 2
                        and not called_func.startswith("_")
                        and called_func not in ("if", "while", "for", "switch", "return")):
                    # Get surrounding code context (3 lines before, 3 after)
                    start_row = node.start_point[0]
                    end_row = node.end_point[0]
                    ctx_start = max(0, start_row - 2)
                    ctx_end = min(len(lines), end_row + 3)
                    snippet = "\n".join(lines[ctx_start:ctx_end])
                    results.append({
                        "func_name": called_func,
                        "file_path": rel_path,
                        "snippet": snippet,
                    })

        for child in node.children:
            _walk(child)

    _walk(root_node)
    return results


def _extract_module_chunks(root_node, source_bytes, rel_path):
    """Extract coarse-grained module-level code chunks.

    Splits a file into logical sections: top-of-file includes/defines,
    then individual function definitions.
    """
    lines = source_bytes.decode("utf-8", errors="replace").split("\n")
    total_lines = len(lines)
    chunks = []

    # Collect all function definitions as chunk boundaries
    func_starts = []
    for node in root_node.children:
        if node.type == "function_definition":
            func_starts.append(node.start_point[0])

    if not func_starts:
        # No functions: take whole file as one chunk
        chunks.append((0, "header", "\n".join(lines)))
        return [
            {"file_path": rel_path, "chunk_id": i, "snippet": text}
            for i, (_, _, text) in enumerate(chunks)
        ]

    # Header chunk (before first function)
    first_func = func_starts[0]
    if first_func > 5:
        header = "\n".join(lines[:first_func])
        chunks.append((0, "header", header))

    # Each function as a chunk
    for i, start_line in enumerate(func_starts):
        end_line = func_starts[i + 1] if i + 1 < len(func_starts) else total_lines
        chunk_text = "\n".join(lines[start_line:end_line])
        if len(chunk_text.strip()) > 10:
            chunks.append((start_line, "function", chunk_text))

    return [
        {"file_path": rel_path, "chunk_id": i, "snippet": text}
        for i, (_, _, text) in enumerate(chunks)
    ]


# ============================================================
# Main build
# ============================================================

def _parse_one(file_path, source_bytes, rel_path):
    """Parse a single C source file and return all extracted info."""
    parser = _get_parser()
    tree = parser.parse(source_bytes)
    root = tree.root_node

    # Check for syntax errors (skip files that tree-sitter can't parse well)
    if root.has_error:
        error_count = sum(1 for n in root.children if n.type == "ERROR")
        if error_count > 3:
            return [], [], [], []

    sigs = _extract_signatures(root, source_bytes, rel_path)
    types = _extract_types(root, source_bytes, rel_path)
    calls = _extract_call_patterns(root, source_bytes, rel_path)
    modules = _extract_module_chunks(root, source_bytes, rel_path)

    return sigs, types, calls, modules


def build_index(source_dir, db_path, index_dir):
    """Build complete RAG index from source tree.

    Args:
        source_dir: root directory of C source code
        db_path:    path for the SQLite database
        index_dir:  directory for FAISS index files
    """
    source_dir = Path(source_dir)
    db_path = Path(db_path)
    db_path.parent.mkdir(parents=True, exist_ok=True)

    # Remove old database
    if db_path.exists():
        db_path.unlink()

    store = IndexStore(db_path, index_dir)
    embedder = Embedder()

    # Collect all data
    all_sigs = []
    all_types = []
    all_calls = []
    all_modules = []

    print(f"Parsing C source files in {source_dir}...")
    count = 0
    for rel_path, abs_path in iter_source_files(source_dir):
        try:
            source_bytes = abs_path.read_bytes()
        except Exception:
            continue

        sigs, types, calls, modules = _parse_one(abs_path, source_bytes, rel_path)

        # Add source_file info
        for s in sigs:
            s["source_file"] = rel_path
        for t in types:
            t["source_file"] = rel_path
        for c in calls:
            c["source_file"] = rel_path
        for m in modules:
            m["source_file"] = rel_path

        all_sigs.extend(sigs)
        all_types.extend(types)
        all_calls.extend(calls)
        all_modules.extend(modules)
        count += 1

        if count % 100 == 0:
            print(f"  ... {count} files, {len(all_sigs)} sigs, "
                  f"{len(all_types)} types, {len(all_calls)} calls, "
                  f"{len(all_modules)} modules")

    print(f"Total: {count} files parsed")
    print(f"  Signatures: {len(all_sigs)}")
    print(f"  Types:      {len(all_types)}")
    print(f"  Calls:      {len(all_calls)}")
    print(f"  Modules:    {len(all_modules)}")

    # Write to SQLite
    print("Writing to database...")
    store.begin()
    for s in all_sigs:
        store.insert_signature(
            s["func_name"], s["file_path"], s["signature"],
            s["return_type"], s["params"], s["doc_comment"],
            s["source_file"],
        )
    for t in all_types:
        store.insert_type(
            t["type_name"], t["kind"], t["definition"],
            t["file_path"], t["source_file"],
        )

    for c in all_calls:
        store.insert_call_pattern(
            c["func_name"], c["file_path"], c["snippet"],
            c["source_file"],
        )
    for m in all_modules:
        store.insert_module_code(
            m["file_path"], m["chunk_id"], m["snippet"],
            m["source_file"],
        )
    store.commit()
    print("Database written.")

    # Build FAISS indices
    if all_calls:
        print(f"Building call-pattern FAISS index ({len(all_calls)} entries)...")
        texts = [c["snippet"][:2000] for c in all_calls]
        vectors = []
        for i in range(0, len(texts), EMBED_BATCH_SIZE):
            batch = texts[i:i + EMBED_BATCH_SIZE]
            vecs = embedder.encode(batch)
            vectors.append(vecs)
        import numpy as np
        all_vectors = np.vstack(vectors) if vectors else np.empty((0, embedder.dim))
        store.build_faiss_call_patterns(all_vectors)

    if all_modules:
        print(f"Building module-code FAISS index ({len(all_modules)} entries)...")
        texts = [m["snippet"][:2000] for m in all_modules]
        vectors = []
        for i in range(0, len(texts), EMBED_BATCH_SIZE):
            batch = texts[i:i + EMBED_BATCH_SIZE]
            vecs = embedder.encode(batch)
            vectors.append(vecs)
        import numpy as np
        all_vectors = np.vstack(vectors) if vectors else np.empty((0, embedder.dim))
        store.build_faiss_module_code(all_vectors)

    store.close()
    print("Index build complete.")

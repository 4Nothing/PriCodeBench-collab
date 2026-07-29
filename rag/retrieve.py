"""Online retrieval logic.

Two-round retrieval:
  Round 1 (exact): function signature + type definitions (SQLite FTS5)
  Round 2 (dense): call-pattern embeddings + module-code embeddings (FAISS)

Output: formatted context text injected before the task prompt.
"""
import re
import os
from pathlib import Path

from rag.config import (
    MAX_SIGNATURES,
    MAX_TYPES,
    MAX_CALL_PATTERNS,
    MAX_MODULE_CODE,
    MAX_CONTEXT_CHARS,
    EMBEDDING_MODEL,
)


def _api_prefix(func_name):
    """Extract the module/API prefix from a function name.

    e.g. 'spi_transfer_bytes' → 'spi', 'gpio_init' → 'gpio'
    """
    # Common RIOT API prefixes
    prefixes = [
        "spi", "i2c", "uart", "gpio", "pwm", "adc", "dac",
        "timer", "rtt", "rtc", "thread", "msg", "mutex", "sem",
        "xtimer", "ztimer", "netif", "gnrc", "sock", "coap",
        "gcoap", "nanocoap", "cbor", "saul", "shell", "fmt",
        "mtd", "flash", "eeprom", "can", "usb", "lora",
        "random", "hashes", "crypto", "cipher", "phydat",
        "periph", "pm", "sched", "irq", "clist", "mbox",
        "kconfig", "log", "l2util", "net", "ieee802154",
        "nrf", "stm", "esp", "cc", "msp", "atm",
    ]
    for p in prefixes:
        if func_name.startswith(p + "_") or func_name.startswith(p + "_"):
            return p
    # Fallback: first segment before underscore
    return func_name.split("_")[0] if "_" in func_name else func_name


def _extract_struct_names(source_path, func_name):
    """Infer likely struct/type names from source path and function name."""
    names = set()

    # From function name: e.g. spi_transfer_bytes → spi_t, spi_params_t
    prefix = _api_prefix(func_name)
    names.add(f"{prefix}_t")
    names.add(f"{prefix}_params_t")
    names.add(f"{prefix}_conf_t")

    # From file name
    stem = Path(source_path).stem
    names.add(f"{stem}_t")
    names.add(f"{stem}_params_t")

    # Generic: params
    names.add("params_t")

    return names


def _round1_signatures(func_name, store):
    """Round 1: FTS5 search for function signatures matching the target function."""
    # Build query: use the function name directly
    query = func_name.replace("_", " OR ") + f" OR {func_name}"
    rows = store.search_signatures_fts(query, limit=MAX_SIGNATURES * 3)

    results = []
    seen_sigs = set()
    for row in rows:
        r_func_name, source_text, return_type, params, doc, file_path, module = row
        # Deduplicate identical signatures
        sig_key = source_text.strip()
        if sig_key in seen_sigs:
            continue
        seen_sigs.add(sig_key)
        results.append({
            "type": "signature",
            "func_name": r_func_name,
            "file": file_path,
            "source": module,
            "content": source_text,
            "return_type": return_type or "",
            "params": params or "",
            "doc": doc or "",
        })
        if len(results) >= MAX_SIGNATURES:
            break
    return results


def _round1_types(struct_names, func_name, source_path, store):
    """Round 1: FTS5 search for struct/type definitions."""
    if not struct_names:
        return []

    # Build type search query
    query_parts = []
    for name in struct_names:
        query_parts.append(name.replace("_", " OR ") + f" OR {name}")
    query = " OR ".join(query_parts)

    rows = store.search_types_fts(query, limit=MAX_TYPES * 3)

    results = []
    seen_defs = set()
    for row in rows:
        type_name, kind, members_text, file_path, module = row
        def_key = members_text.strip()[:200]
        if def_key in seen_defs:
            continue
        seen_defs.add(def_key)
        results.append({
            "type": "type_def",
            "name": type_name,
            "kind": kind,
            "file": file_path,
            "source": module,
            "content": members_text,
        })
        if len(results) >= MAX_TYPES:
            break
    return results


def _round2_dense(func_name, source_path, store, embedder):
    """Round 2: dense semantic search on call patterns and module code."""
    query_vec = embedder.encode(func_name)

    results = []

    # --- Call patterns ---
    call_hits = store.faiss_search_call_patterns(query_vec, k=MAX_CALL_PATTERNS * 2)
    seen_snippets = set()
    for score, row_id in call_hits:
        row = store.get_call_pattern_by_id(row_id)
        if row is None:
            continue
        _, cp_func_name, file_path, code_snippet, line_number, module = row
        snippet_key = code_snippet.strip()[:200]
        if snippet_key in seen_snippets:
            continue
        seen_snippets.add(snippet_key)
        results.append({
            "type": "call_pattern",
            "func_name": cp_func_name,
            "file": file_path,
            "source": module,
            "content": code_snippet,
            "score": score,
        })
        if len([r for r in results if r["type"] == "call_pattern"]) >= MAX_CALL_PATTERNS:
            break

    # --- Module code ---
    module_hits = store.faiss_search_module_code(query_vec, k=MAX_MODULE_CODE * 2)
    for score, row_id in module_hits:
        row = store.get_module_code_by_id(row_id)
        if row is None:
            continue
        _, func_name, file_path, code_snippet, module = row
        snippet_key = code_snippet.strip()[:200]
        if snippet_key in seen_snippets:
            continue
        seen_snippets.add(snippet_key)
        results.append({
            "type": "module_code",
            "file": file_path,
            "chunk": 0,
            "source": module,
            "content": code_snippet,
            "score": score,
        })
        if len([r for r in results if r["type"] == "module_code"]) >= MAX_MODULE_CODE:
            break

    return results


def _format_context(round1, round2, target_file):
    """Format retrieved results into a prompt context block."""
    lines = []
    lines.append("/*")
    lines.append(" * ===== RAG RETRIEVED CONTEXT =====")
    lines.append(" * The following code snippets were retrieved from the codebase")
    lines.append(" * and may help you understand the APIs and patterns used here.")
    lines.append(" */")
    lines.append("")

    total_chars = 0

    # Round 1 results first (most relevant)
    if round1:
        for r in round1:
            if r["type"] == "signature":
                block = f"// Function: {r['func_name']} (from {r['file']})\n"
                if r.get("return_type"):
                    block += f"// Returns: {r['return_type']}\n"
                if r.get("params"):
                    block += f"// Params: {r['params']}\n"
                if r.get("doc"):
                    block += f"// Doc: {r['doc']}\n"
                block += f"{r['content']}\n"
            elif r["type"] == "type_def":
                block = f"// Type: {r['name']} ({r['kind']}) (from {r['file']})\n"
                block += f"{r['content']}\n"
            else:
                continue

            if total_chars + len(block) > MAX_CONTEXT_CHARS:
                break
            lines.append(block)
            total_chars += len(block)

    # Round 2 results
    if round2:
        for r in round2:
            if r["type"] == "call_pattern":
                block = (
                    f"// Call pattern: {r['func_name']} "
                    f"(from {r['file']}, similarity={r['score']:.2f})\n"
                    f"{r['content']}\n"
                )
            elif r["type"] == "module_code":
                block = (
                    f"// Module code (from {r['file']}, "
                    f"similarity={r['score']:.2f})\n"
                    f"{r['content']}\n"
                )
            else:
                continue

            if total_chars + len(block) > MAX_CONTEXT_CHARS:
                break
            lines.append(block)
            total_chars += len(block)

    if total_chars == 0:
        return ""

    return "\n".join(lines)


def retrieve(sut_function, source_path, store, embedder):
    """Main retrieval entry point.

    Args:
        sut_function: target function name (e.g. "mtd_init")
        source_path:  relative source file path (for context)
        store:        IndexStore instance
        embedder:     Embedder instance

    Returns:
        str or None: formatted RAG context to prepend to the prompt,
                     or None if nothing retrieved.
    """
    # Round 1: exact match
    r1_sig = _round1_signatures(sut_function, store)
    struct_names = _extract_struct_names(source_path, sut_function)
    r1_types = _round1_types(struct_names, sut_function, source_path, store)
    round1 = r1_sig + r1_types

    # Round 2: dense semantic
    round2 = _round2_dense(sut_function, source_path, store, embedder)

    if not round1 and not round2:
        return None

    context = _format_context(round1, round2, source_path)
    return context or None

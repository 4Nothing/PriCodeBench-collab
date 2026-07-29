"""
SQLite FTS5 + FAISS storage for RAG indices.

Indices:
  - signatures FTS5: function signature text (BM25 exact/fuzzy match)
  - types FTS5: struct/enum/union type definitions
  - call_patterns + FAISS: call-site patterns (dense semantic)
  - module_code + FAISS: module-level code snippets (dense semantic)
"""
import os
import sqlite3
import numpy as np
from pathlib import Path

import faiss


class IndexStore:
    def __init__(self, db_path, index_dir):
        self.db_path = str(db_path)
        self.index_dir = Path(index_dir)
        self.index_dir.mkdir(parents=True, exist_ok=True)

        # WSL Plan 9 filesystem workaround: use DELETE journal mode
        # to avoid SQLite shared-memory / WAL locking issues
        self._conn = sqlite3.connect(self.db_path)
        self._conn.execute("PRAGMA journal_mode=DELETE")
        self._conn.execute("PRAGMA synchronous=OFF")
        self._conn.execute("PRAGMA cache_size=-8000")

        self._init_schema()
        self._load_faiss()

        self._ablation = None  # None=all indices, or set of disabled index names

    def set_ablation(self, ablation_str):
        """Disable specified index backends for ablation study.

        Args:
            ablation_str: comma-separated list of index names to DISABLE.
                          e.g. "signatures,types" disables SQLite FTS5,
                          leaving only FAISS indices.
        """
        self._ablation = set(ablation_str.strip().split(",")) if ablation_str else set()

    def _enabled(self, name):
        return self._ablation is None or name not in self._ablation

    def _init_schema(self):
        cur = self._conn.cursor()
        # Function signatures table + FTS5 virtual table
        cur.execute("""
            CREATE TABLE IF NOT EXISTS signatures (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                func_name TEXT NOT NULL,
                file_path TEXT NOT NULL,
                signature TEXT NOT NULL,
                return_type TEXT,
                params TEXT,
                doc_comment TEXT,
                source_file TEXT NOT NULL
            )
        """)
        cur.execute(
            "CREATE VIRTUAL TABLE IF NOT EXISTS signatures_fts "
            "USING fts5(func_name, signature, return_type, params, doc_comment, "
            "content='signatures', content_rowid='id')"
        )

        # Type definitions table + FTS5
        cur.execute("""
            CREATE TABLE IF NOT EXISTS types (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                type_name TEXT NOT NULL,
                kind TEXT NOT NULL,
                definition TEXT NOT NULL,
                file_path TEXT NOT NULL,
                source_file TEXT NOT NULL
            )
        """)
        cur.execute(
            "CREATE VIRTUAL TABLE IF NOT EXISTS types_fts "
            "USING fts5(type_name, kind, definition, "
            "content='types', content_rowid='id')"
        )

        # Call pattern table (text + FAISS vector ID)
        cur.execute("""
            CREATE TABLE IF NOT EXISTS call_patterns (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                func_name TEXT NOT NULL,
                file_path TEXT NOT NULL,
                snippet TEXT NOT NULL,
                source_file TEXT NOT NULL
            )
        """)

        # Module code table (text + FAISS vector ID)
        cur.execute("""
            CREATE TABLE IF NOT EXISTS module_code (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                file_path TEXT NOT NULL,
                chunk_id INTEGER NOT NULL,
                snippet TEXT NOT NULL,
                source_file TEXT NOT NULL
            )
        """)

        self._conn.commit()

    # --- FAISS ---

    def _faiss_path(self, name):
        return self.index_dir / f"{name}.faiss"

    def _load_faiss(self):
        self._faiss_call = self._load_or_create_faiss("call_patterns")
        self._faiss_module = self._load_or_create_faiss("module_code")

    def _load_or_create_faiss(self, name):
        p = self._faiss_path(name)
        if p.exists():
            return faiss.read_index(str(p))
        return None

    def save_faiss(self, name, index):
        faiss.write_index(index, str(self._faiss_path(name)))

    # --- Write ---

    def begin(self):
        """Begin batch write transaction."""
        self._conn.execute("BEGIN")

    def commit(self):
        self._conn.commit()
        # Rebuild FTS content
        self._conn.execute("INSERT INTO signatures(signatures_fts) VALUES('rebuild')")
        self._conn.execute("INSERT INTO types(types_fts) VALUES('rebuild')")

    def insert_signature(self, func_name, file_path, signature, return_type,
                         params, doc_comment, source_file):
        cur = self._conn.cursor()
        cur.execute(
            "INSERT INTO signatures(func_name, file_path, signature, "
            "return_type, params, doc_comment, source_file) "
            "VALUES (?, ?, ?, ?, ?, ?, ?)",
            (func_name, file_path, signature, return_type, params,
             doc_comment, source_file),
        )
        return cur.lastrowid

    def insert_type(self, type_name, kind, definition, file_path, source_file):
        cur = self._conn.cursor()
        cur.execute(
            "INSERT INTO types(type_name, kind, definition, file_path, source_file) "
            "VALUES (?, ?, ?, ?, ?)",
            (type_name, kind, definition, file_path, source_file),
        )
        return cur.lastrowid

    def insert_call_pattern(self, func_name, file_path, snippet, source_file):
        cur = self._conn.cursor()
        cur.execute(
            "INSERT INTO call_patterns(func_name, file_path, snippet, source_file) "
            "VALUES (?, ?, ?, ?)",
            (func_name, file_path, snippet, source_file),
        )
        return cur.lastrowid

    def insert_module_code(self, file_path, chunk_id, snippet, source_file):
        cur = self._conn.cursor()
        cur.execute(
            "INSERT INTO module_code(file_path, chunk_id, snippet, source_file) "
            "VALUES (?, ?, ?, ?)",
            (file_path, chunk_id, snippet, source_file),
        )
        return cur.lastrowid

    # --- Query helpers ---

    def search_signatures_fts(self, query, limit=10):
        """FTS5 full-text search on function signatures."""
        if not self._enabled("signatures"):
            return []
        cur = self._conn.cursor()
        try:
            cur.execute(
                "SELECT s.func_name, s.source_text, s.return_type, s.params, s.doc, "
                "s.file_path, s.module "
                "FROM signatures s WHERE s.id IN ("
                "  SELECT rowid FROM signatures_fts WHERE signatures_fts MATCH ?"
                ") LIMIT ?",
                (query, limit),
            )
            return cur.fetchall()
        except sqlite3.OperationalError:
            return []

    def search_types_fts(self, query, limit=10):
        """FTS5 full-text search on type definitions."""
        if not self._enabled("types"):
            return []
        cur = self._conn.cursor()
        try:
            cur.execute(
                "SELECT t.type_name, t.kind, t.members_text, t.file_path, t.module "
                "FROM types t WHERE t.id IN ("
                "  SELECT rowid FROM types_fts WHERE types_fts MATCH ?"
                ") LIMIT ?",
                (query, limit),
            )
            return cur.fetchall()
        except sqlite3.OperationalError:
            return []

    def get_all_call_patterns(self):
        """Return all call-pattern rows: (id, func_name, file_path, code_snippet, line_number, module)."""
        cur = self._conn.cursor()
        cur.execute("SELECT id, func_name, file_path, code_snippet, line_number, module "
                    "FROM call_patterns")
        return cur.fetchall()

    def get_all_module_code(self):
        """Return all module-code rows: (id, func_name, file_path, code_snippet, module)."""
        cur = self._conn.cursor()
        cur.execute("SELECT id, func_name, file_path, code_snippet, module "
                    "FROM module_code")
        return cur.fetchall()

    def faiss_search_call_patterns(self, query_vector, k=5):
        """Dense semantic search on call-pattern vectors."""
        if not self._enabled("call_patterns"):
            return []
        if self._faiss_call is None:
            return []
        query_vec = np.asarray(query_vector, dtype=np.float32).reshape(1, -1)
        D, I = self._faiss_call.search(query_vec, k)
        return [(float(D[0][i]), int(I[0][i])) for i in range(len(I[0]))
                if I[0][i] >= 0]

    def faiss_search_module_code(self, query_vector, k=5):
        """Dense semantic search on module-code vectors."""
        if not self._enabled("module_code"):
            return []
        if self._faiss_module is None:
            return []
        query_vec = np.asarray(query_vector, dtype=np.float32).reshape(1, -1)
        D, I = self._faiss_module.search(query_vec, k)
        return [(float(D[0][i]), int(I[0][i])) for i in range(len(I[0]))
                if I[0][i] >= 0]

    def get_call_pattern_by_id(self, row_id):
        cur = self._conn.cursor()
        cur.execute("SELECT id, func_name, file_path, code_snippet, line_number, module "
                    "FROM call_patterns WHERE id = ?", (row_id,))
        return cur.fetchone()

    def get_module_code_by_id(self, row_id):
        cur = self._conn.cursor()
        cur.execute("SELECT id, func_name, file_path, code_snippet, module "
                    "FROM module_code WHERE id = ?", (row_id,))
        return cur.fetchone()

    # --- Build FAISS indices ---

    def build_faiss_call_patterns(self, vectors):
        """Build and save FAISS index for call patterns. vectors = N x dim float32."""
        if len(vectors) == 0:
            return
        dim = vectors.shape[1]
        index = faiss.IndexFlatIP(dim)
        index.add(vectors)
        self._faiss_call = index
        self.save_faiss("call_patterns", index)

    def build_faiss_module_code(self, vectors):
        """Build and save FAISS index for module code."""
        if len(vectors) == 0:
            return
        dim = vectors.shape[1]
        index = faiss.IndexFlatIP(dim)
        index.add(vectors)
        self._faiss_module = index
        self.save_faiss("module_code", index)

    def close(self):
        self._conn.close()

"""
RAG (Retrieval-Augmented Generation) for OS code completion.

Provides:
  - Embedder:  sentence-transformers dense embedding
  - IndexStore: SQLite FTS5 + FAISS storage
  - build_index: offline index construction
  - retrieve:   online retrieval + prompt formatting
"""
from rag.embedder import Embedder
from rag.index_store import IndexStore
from rag.build_index import build_index, index_exists
from rag.retrieve import retrieve

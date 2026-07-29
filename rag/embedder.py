"""Dense embedding via sentence-transformers"""
import numpy as np
import os
from rag.config import EMBEDDING_MODEL, EMBED_BATCH_SIZE


class Embedder:
    def __init__(self, model_name=None):
        from sentence_transformers import SentenceTransformer
        _model_name = model_name or EMBEDDING_MODEL
        cache_dir = os.path.expanduser(
            "~/.cache/huggingface/hub/models--sentence-transformers--"
            + _model_name.replace("/", "--")
        )
        local_only = os.path.isdir(cache_dir)
        self.model = SentenceTransformer(_model_name, local_files_only=local_only)

    def encode(self, texts):
        """Encode a list of texts to a float32 array."""
        if isinstance(texts, str):
            texts = [texts]
        if not texts:
            return np.empty((0, self.model.get_sentence_embedding_dimension()),
                            dtype=np.float32)
        embeddings = self.model.encode(
            texts,
            batch_size=EMBED_BATCH_SIZE,
            show_progress_bar=False,
            convert_to_numpy=True,
            normalize_embeddings=True,
        )
        return embeddings.astype(np.float32)

    @property
    def dim(self):
        return self.model.get_sentence_embedding_dimension()

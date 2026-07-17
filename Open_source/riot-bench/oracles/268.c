void sha256(const void *data, size_t len, void *digest)
{
    sha256_context_t c;
    assert(digest);

    sha256_init(&c);
    sha256_update(&c, data, len);
    sha256_final(&c, digest);
}

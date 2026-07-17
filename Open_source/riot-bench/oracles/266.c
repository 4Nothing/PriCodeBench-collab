void sha224(const void *data, size_t len, void *digest)
{
    sha224_context_t c;
    assert(digest);

    sha224_init(&c);
    sha224_update(&c, data, len);
    sha224_final(&c, digest);
}
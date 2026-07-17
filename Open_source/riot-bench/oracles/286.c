void sha512_224(const void *data, size_t len, void *digest)
{
    sha512_224_context_t c;
    assert(digest);

    sha512_224_init(&c);
    sha512_224_update(&c, data, len);
    sha512_224_final(&c, digest);
}

void sha512_256(const void *data, size_t len, void *digest)
{
    sha512_256_context_t c;
    assert(digest);

    sha512_256_init(&c);
    sha512_256_update(&c, data, len);
    sha512_256_final(&c, digest);
}

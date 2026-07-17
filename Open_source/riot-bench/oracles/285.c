void sha512(const void *data, size_t len, void *digest)
{
    sha512_context_t c;
    assert(digest);

    sha512_init(&c);
    sha512_update(&c, data, len);
    sha512_final(&c, digest);
}

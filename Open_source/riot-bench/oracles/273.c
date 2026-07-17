void sha384(const void *data, size_t len, void *digest)
{
    sha384_context_t c;
    assert(digest);

    sha384_init(&c);
    sha384_update(&c, data, len);
    sha384_final(&c, digest);
}

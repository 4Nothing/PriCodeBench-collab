void sha1(void *digest, const void *data, size_t len)
{
    sha1_context ctx;

    sha1_init(&ctx);
    sha1_update(&ctx, (unsigned char *) data, len);
    sha1_final(&ctx, digest);
}

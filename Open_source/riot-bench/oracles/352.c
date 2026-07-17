void uuid_v5(uuid_t *uuid, const uuid_t *ns, const uint8_t *name, size_t len)
{
    uint8_t digest[20];
    sha1_context ctx;

    sha1_init(&ctx);
    sha1_update(&ctx, ns, sizeof(uuid_t));
    sha1_update(&ctx, name, len);
    sha1_final(&ctx, digest);

    memcpy(uuid, digest, sizeof(uuid_t));

    _set_version(uuid, UUID_V5);
    _set_reserved(uuid);
}

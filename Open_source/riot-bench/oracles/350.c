void uuid_v3(uuid_t *uuid, const uuid_t *ns, const uint8_t *name, size_t len)
{
    /* Digest calculation */
    md5_ctx_t ctx;

    md5_init(&ctx);
    md5_update(&ctx, ns, sizeof(uuid_t));
    md5_update(&ctx, name, len);
    md5_final(&ctx, uuid);

    _set_version(uuid, UUID_V3);
    _set_reserved(uuid);
}

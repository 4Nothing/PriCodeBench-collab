void uuid_v4(uuid_t *uuid)
{
    random_bytes((uint8_t *)uuid, sizeof(uuid_t));
    _set_version(uuid, UUID_V4);
    _set_reserved(uuid);
}

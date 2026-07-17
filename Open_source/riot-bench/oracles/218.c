void luid_get_eui48(eui48_t *addr)
{
    luid_base(addr, sizeof(*addr));
    addr->uint8[5] ^= lastused++;

    eui48_set_local(addr);
    eui48_clear_group(addr);
}

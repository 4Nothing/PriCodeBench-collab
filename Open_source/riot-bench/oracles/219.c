void luid_get_eui64(eui64_t *addr)
{
    luid_base(addr, sizeof(*addr));
    addr->uint8[7] ^= lastused++;

    eui64_set_local(addr);
    eui64_clear_group(addr);
}

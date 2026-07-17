void bluetil_addr_sprint(char *out, const uint8_t *addr)
{
    assert(out);
    assert(addr);

    fmt_byte_hex(out, addr[0]);
    for (unsigned i = 1; i < BLE_ADDR_LEN; i++) {
        out += 2;
        *out++ = ':';
        fmt_byte_hex(out, addr[i]);
    }
    out += 2;
    *out = '\0';
}

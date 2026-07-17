void bluetil_addr_ipv6_l2ll_sprint(char *out, const uint8_t *addr)
{
    assert(out);
    assert(addr);

    memcpy(out, "[FE80::", 7);
    out += 7;
    out += fmt_byte_hex(out, addr[0]);
    out += fmt_byte_hex(out, addr[1]);
    *out++ = ':';
    out += fmt_byte_hex(out, addr[2]);
    memcpy(out, "FF:FE", 5);
    out += 5;
    out += fmt_byte_hex(out, addr[3]);
    *out++ = ':';
    out += fmt_byte_hex(out, addr[4]);
    out += fmt_byte_hex(out, addr[5]);
    *out++ = ']';
    *out = '\0';
}

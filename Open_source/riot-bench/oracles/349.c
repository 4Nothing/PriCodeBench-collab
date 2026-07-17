void uuid_to_string(const uuid_t *uuid, char *str)
{
    char *p = str;
    p += fmt_u32_hex(p, byteorder_ntohl(uuid->time_low));
    p += fmt_char(p, '-');
    p += fmt_u16_hex(p, byteorder_ntohs(uuid->time_mid));
    p += fmt_char(p, '-');
    p += fmt_u16_hex(p, byteorder_ntohs(uuid->time_hi));
    p += fmt_char(p, '-');
    p += fmt_byte_hex(p, uuid->clk_seq_hi_res);
    p += fmt_byte_hex(p, uuid->clk_seq_low);
    p += fmt_char(p, '-');
    p += fmt_bytes_hex(p, uuid->node, UUID_NODE_LEN);
    *p = '\0';
    fmt_to_lower(str, str);
}

int uuid_from_string(uuid_t *uuid, const char *str)
{
    uint32_t tmp;
    if (fmt_strlen(str) < UUID_STR_LEN) {
        return -1;
    }
    tmp = scn_u32_hex(str, 8);
    uuid->time_low = byteorder_htonl(tmp);
    str += 8;
    if (*str++ != '-') {
        return -2;
    }
    tmp = scn_u32_hex(str, 4);
    uuid->time_mid = byteorder_htons(tmp);
    str += 4;
    if (*str++ != '-') {
        return -2;
    }
    tmp = scn_u32_hex(str, 4);
    uuid->time_hi = byteorder_htons(tmp);
    str += 4;
    if (*str++ != '-') {
        return -2;
    }
    uuid->clk_seq_hi_res = scn_u32_hex(str, 2);
    str += 2;
    uuid->clk_seq_low = scn_u32_hex(str, 2);
    str += 2;
    if (*str++ != '-') {
        return -2;
    }
    for (unsigned i = 0; i < UUID_NODE_LEN; i++) {
        uuid->node[i] = scn_u32_hex(str, 2);
        str += 2;
    }

    return 0;
}

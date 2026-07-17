void color_rgb2str(const color_rgb_t *rgb, char* str)
{
    uint8_t tmp;

    /* RR */
    tmp = rgb->r >> 4;
    str[0] = (tmp > 9) ? ('A' - 10 + tmp) : ('0' + tmp);
    tmp = rgb->r & 0x0F;
    str[1] = (tmp > 9) ? ('A' - 10 + tmp) : ('0' + tmp);

    /* GG */
    tmp = rgb->g >> 4;
    str[2] = (tmp > 9) ? ('A' - 10 + tmp) : ('0' + tmp);
    tmp = rgb->g & 0x0F;
    str[3] = (tmp > 9) ? ('A' - 10 + tmp) : ('0' + tmp);

    /* BB */
    tmp = rgb->b >> 4;
    str[4] = (tmp > 9) ? ('A' - 10 + tmp) : ('0' + tmp);
    tmp = rgb->b & 0x0F;
    str[5] = (tmp > 9) ? ('A' - 10 + tmp) : ('0' + tmp);
}

v
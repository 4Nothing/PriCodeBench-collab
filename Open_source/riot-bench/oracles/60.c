void color_rgb2hex(const color_rgb_t *rgb, uint32_t *hex)
{
    *hex = (((uint32_t) rgb->r) << 16UL) | (rgb->g << 8UL) | (rgb->b);
}

v
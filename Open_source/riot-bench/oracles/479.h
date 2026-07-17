static inline void color_rgb_invert(const color_rgb_t *rgb, color_rgb_t *inv_rgb)
{
    inv_rgb->r = rgb->r ^ 0xFF;
    inv_rgb->g = rgb->g ^ 0xFF;
    inv_rgb->b = rgb->b ^ 0xFF;
}

/
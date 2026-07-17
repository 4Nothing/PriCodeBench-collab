void color_rgb_complementary(const color_rgb_t *rgb, color_rgb_t *comp_rgb)
{
    uint8_t  max = 0;
    uint8_t  min = 0;
    uint16_t val = 0;

    max = (rgb->r > rgb->g) ? rgb->r : rgb->g;
    max = (rgb->b > max) ? rgb->b : max;
    min = (rgb->r < rgb->g) ? rgb->r : rgb->g;
    min = (rgb->b < min) ? rgb->b : min;
    val = max + min;

    comp_rgb->r = val - rgb->r;
    comp_rgb->g = val - rgb->g;
    comp_rgb->b = val - rgb->b;
}

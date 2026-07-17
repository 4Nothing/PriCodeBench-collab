void color_rgb2hsv(color_rgb_t *rgb, color_hsv_t *hsv)
{
    float rd, gd, bd, delta, min, max;
    int imax, imin, sector;

    /* catch special case grey first */
    if (rgb->r == rgb->g && rgb->r == rgb->b) {
        hsv->v = (float)rgb->r * (1 / 255.0f);
        hsv->s = 0.0f;
        hsv->h = 0.0f; /* hue might be anything for grey, but it is not uncommon to use 0 */
        return;
    }

    /* normalize RGB colors to the range [0 - 1.0] */
    /* multiplication is often faster than division -> using compile time constant */
    rd = (float)rgb->r * (1 / 255.0f);
    gd = (float)rgb->g * (1 / 255.0f);
    bd = (float)rgb->b * (1 / 255.0f);

    /* find maximum of the three colors and sector */
    /* using the comparing faster integer color value */
    imax = rgb->r;
    max = rd;
    sector = 0;
    if (rgb->g > imax) {
        imax = rgb->g;
        max = gd;
        sector = 1;
    }
    if (rgb->b > imax) {
        imax = rgb->b;
        max = bd;
        sector = 2;
    }
    /* value is maximum*/
    hsv->v = max;

    /* find of minimum the three RGB colors */
    imin = rgb->r;
    min = rd;
    if (rgb->g < imin) {
        imin = rgb->g;
        min = gd;
    }
    if (rgb->b < imin) {
        imin = rgb->b;
        min = bd;
    }
    /* compute delta from value and minimum*/
    delta = hsv->v - min;

    /* find the saturation from value and delta */
    /* special case gray r == g == b ^= min == max */
    hsv->s = delta / max;

    /* compute hue */
    float p = 60.0f / delta;
    switch (sector){
        case 0:
            hsv->h = (gd - bd) * p;
            break;
        case 1:
            hsv->h = 120.0f + (bd - rd) * p;
            break;
        case 2:
            hsv->h = 240.0f + (rd - gd) * p;
            break;
    }
    if (hsv->h < 0.0f) {
        hsv->h += 360.0f;
    }
}

v
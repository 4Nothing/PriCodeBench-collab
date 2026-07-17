void color_hsv2rgb(color_hsv_t *hsv, color_rgb_t *rgb)
{
    int i;
    float aa, bb, cc, f, h;

    if (hsv->s == 0.0f) {
        rgb->r = (uint8_t)(hsv->v * 255.0f);
        rgb->g = (uint8_t)(hsv->v * 255.0f);
        rgb->b = (uint8_t)(hsv->v * 255.0f);
        return;
    }

    h = (hsv->h == 360.0f) ? 0.0f : hsv->h;
    h /= 60.0f;
    i = (int)h;
    f = h - i;
    aa = hsv->v * (1.0f - hsv->s);
    bb = hsv->v * (1.0f - (hsv->s * f));
    cc = hsv->v * (1.0f - (hsv->s * (1.0f - f)));

    switch (i) {
        case 0:
            rgb->r = (uint8_t)(hsv->v * 255.0f);
            rgb->g = (uint8_t)(cc * 255.0f);
            rgb->b = (uint8_t)(aa * 255.0f);
            break;
        case 1:
            rgb->r = (uint8_t)(bb * 255.0f);
            rgb->g = (uint8_t)(hsv->v * 255.0f);
            rgb->b = (uint8_t)(aa * 255.0f);
            break;
        case 2:
            rgb->r = (uint8_t)(aa * 255.0f);
            rgb->g = (uint8_t)(hsv->v * 255.0f);
            rgb->b = (uint8_t)(cc * 255.0f);
            break;
        case 3:
            rgb->r = (uint8_t)(aa * 255.0f);
            rgb->g = (uint8_t)(bb * 255.0f);
            rgb->b = (uint8_t)(hsv->v * 255.0f);
            break;
        case 4:
            rgb->r = (uint8_t)(cc * 255.0f);
            rgb->g = (uint8_t)(aa * 255.0f);
            rgb->b = (uint8_t)(hsv->v * 255.0f);
            break;
        case 5:
            rgb->r = (uint8_t)(hsv->v * 255.0f);
            rgb->g = (uint8_t)(aa * 255.0f);
            rgb->b = (uint8_t)(bb * 255.0f);
            break;
    }
}

v
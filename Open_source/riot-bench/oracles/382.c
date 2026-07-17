void ztimer_convert_muldiv64_init(
    ztimer_convert_muldiv64_t *ztimer_convert_muldiv64, ztimer_clock_t *lower,
    unsigned div, unsigned mul)
{
    uint32_t max_value;

    if (mul > div) {
        max_value = (uint64_t)UINT32_MAX * div / mul;
    }
    else {
        max_value = UINT32_MAX;
    }

    DEBUG(
        "ztimer_convert_muldiv64_init() div=%u mul=%u lower_maxval=%" PRIu32 "\n",
        div, mul, max_value);

    ztimer_convert_init(&ztimer_convert_muldiv64->super, lower, max_value);
    ztimer_convert_muldiv64->super.super.ops = &_ztimer_convert_muldiv64_ops;
    ztimer_convert_muldiv64->div = div;
    ztimer_convert_muldiv64->mul = mul;
#if !MODULE_ZTIMER_ONDEMAND
    /* extend lower clock only if the ondemand driver isn't selected
     * otherwise, the clock extension will be called with the first
     * ztimer_acquire() call */
    ztimer_init_extend(&ztimer_convert_muldiv64->super.super);
#endif
}

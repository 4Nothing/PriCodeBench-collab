void ztimer_convert_frac_init(ztimer_convert_frac_t *self,
                              ztimer_clock_t *lower, uint32_t freq_self,
                              uint32_t freq_lower)
{
    DEBUG("ztimer_convert_frac_init: %p->%p fs=%" PRIu32 " fl=%" PRIu32 "\n",
          (void *)self, (void *)lower, freq_self, freq_lower);

    *self = (ztimer_convert_frac_t) {
        .super.super = { .ops = &ztimer_convert_frac_ops, },
        .super.lower = lower,
        .super.lower_entry =
        { .callback = (void (*)(void *)) ztimer_handler, .arg = &self->super, },
    };

    ztimer_convert_frac_compute_scale(self, freq_self, freq_lower);
    if (freq_self < freq_lower) {
        self->super.super.max_value = frac_scale(&self->scale_now, UINT32_MAX);
#if !MODULE_ZTIMER_ONDEMAND
        /* extend lower clock only if the ondemand driver isn't selected
         * otherwise, the clock extension will be called with the first
         * ztimer_acquire() call */
        ztimer_init_extend(&self->super.super);
#endif
    }
    else {
        DEBUG("ztimer_convert_frac_init: rounding up val:%" PRIu32 "\n",
              (uint32_t)(freq_self / freq_lower));
        self->round = freq_self / freq_lower;
        self->super.super.max_value = UINT32_MAX;
    }
#if MODULE_PM_LAYERED && !MODULE_ZTIMER_ONDEMAND
    self->super.super.block_pm_mode = ZTIMER_CLOCK_NO_REQUIRED_PM_MODE;
#endif
}

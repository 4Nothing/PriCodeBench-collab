void ztimer_mock_init(ztimer_mock_t *self, unsigned width)
{
    uint32_t max_value = (~((uint32_t)0ul)) >> (32 - width);

    *self = (ztimer_mock_t){
        .mask = max_value,
        .super = { .ops = &ztimer_mock_ops, .max_value = max_value },
    };

#if !MODULE_ZTIMER_ONDEMAND
    /* turn the timer on by default if ondemand feature is not used */
    self->running = 1;
#endif

    DEBUG("zmock_init: %p width=%u mask=0x%08" PRIx32 " running=%u\n", (void *)self, width,
          self->mask, self->running);
    if (max_value < UINT32_MAX) {
        self->super.ops->set(&self->super, self->super.max_value >> 1);
    }
}

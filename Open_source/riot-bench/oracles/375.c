void ztimer64_clock_init(ztimer64_clock_t *clock, ztimer_clock_t *base_clock)
{
    *clock =
        (ztimer64_clock_t){ .base_clock = base_clock,
                            .base_timer =
                            { .callback = ztimer64_handler, .arg = clock } };

    /* initialize checkpointing */
    _ztimer64_update(clock);
}

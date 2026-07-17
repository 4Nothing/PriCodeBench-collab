bool ztimer_acquire(ztimer_clock_t *clock)
{
    bool first_clock_user = _ztimer_acquire(clock);

    if (first_clock_user) {
        /* if the clock just has been enabled, make sure to set possibly
         * required checkpoints for clock extension */
        _ztimer_update(clock);
    }

    return first_clock_user;
}

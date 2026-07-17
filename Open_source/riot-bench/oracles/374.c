void xtimer_init(void)
{
#ifndef MODULE_XTIMER_ON_ZTIMER
    /* initialize low-level timer */
    int ret = timer_init(XTIMER_DEV, XTIMER_HZ, _periph_timer_callback, NULL);
    (void)ret;
    assert(ret == 0);
#endif

    /* register initial overflow tick */
    _schedule_earliest_lltimer(_xtimer_now());
}

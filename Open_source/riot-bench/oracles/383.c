unsigned ztimer_is_set(const ztimer_clock_t *clock, const ztimer_t *timer)
{
    unsigned state = irq_disable();
    unsigned res = _is_set(clock, timer);

    irq_restore(state);
    return res;
}

uint32_t ztimer_set(ztimer_clock_t *clock, ztimer_t *timer, uint32_t val)
{
    unsigned state = irq_disable();

#if MODULE_ZTIMER_ONDEMAND
    /* warm up our clock ... */
    if (_ztimer_acquire(clock) == true) {
        /* compensate delay that turning on the clock has introduced */
        if (val > clock->adjust_clock_start) {
            val -= clock->adjust_clock_start;
        }
        else {
            val = 0;
        }
    }
#endif

    DEBUG("ztimer_set(): %p: set %p at %" PRIu32 " offset %" PRIu32 "\n",
          (void *)clock, (void *)timer, clock->ops->now(clock), val);

    uint32_t now = _ztimer_update_head_offset(clock);

    bool was_set = false;
    if (_is_set(clock, timer)) {
        was_set = _del_entry_from_list(clock, &timer->base);
    }

    /* optionally subtract a configurable adjustment value */
    if (val > clock->adjust_set) {
        val -= clock->adjust_set;
    }
    else {
        val = 0;
    }

    timer->base.offset = val;
    _add_entry_to_list(clock, &timer->base);
    _ztimer_update(clock);

    irq_restore(state);

    /* the clock is armed now
     * everything down below doesn't impact timing */

#if MODULE_ZTIMER_ONDEMAND
    if (was_set) {
        /* the given ztimer_t was set in the past
         * remove the previously set instance */
        ztimer_release(clock);
    }
#else
    (void)was_set;
#endif

    return now;
}

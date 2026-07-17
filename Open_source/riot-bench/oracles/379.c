void ztimer64_set_at(ztimer64_clock_t *clock, ztimer64_t *timer, uint64_t val)
{
    unsigned state = irq_disable();

    if (_is_set(timer)) {
        _del_entry_from_list(clock, &timer->base);
    }

    /* optionally subtract a configurable adjustment value */
    /* there is no API to set this ATM */
    if (val > clock->adjust_set) {
        val -= clock->adjust_set;
    }
    else {
        val = 0;
    }

    timer->base.target = val;

    /* NOTE: if val is 0 at this point, this will be fixed to "now()" in _ztimer64_update() */

    if (_add_entry_to_list(clock, &timer->base)) {
        _ztimer64_update(clock);
    }

    irq_restore(state);
}

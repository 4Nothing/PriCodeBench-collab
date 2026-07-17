void ztimer64_remove(ztimer64_clock_t *clock, ztimer64_t *timer)
{
    unsigned state = irq_disable();

    if (_is_set(timer)) {
        if (_del_entry_from_list(clock, &timer->base) == 0) {
            _ztimer64_update(clock);
        }
    }

    irq_restore(state);
}

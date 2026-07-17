bool ztimer_remove(ztimer_clock_t *clock, ztimer_t *timer)
{
    bool was_removed = false;
    bool no_clock_user_left = false;
    unsigned state = irq_disable();

    if (_is_set(clock, timer)) {
        _ztimer_update_head_offset(clock);
        was_removed = _del_entry_from_list(clock, &timer->base);

#if MODULE_ZTIMER_ONDEMAND
        if (was_removed) {
            no_clock_user_left = ztimer_release(clock);
        }
#endif

        if (!no_clock_user_left) {
            _ztimer_update(clock);
        }
    }

    irq_restore(state);
    return was_removed;
}

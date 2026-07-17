void ztimer_sleep(ztimer_clock_t *clock, uint32_t duration)
{
    assert(!irq_is_in());
    mutex_t mutex = MUTEX_INIT_LOCKED;

    ztimer_t timer = {
        .callback = _callback_unlock_mutex,
        .arg = (void *)&mutex,
    };

    /* correct board / MCU specific overhead */
    if (duration > clock->adjust_sleep) {
        duration -= clock->adjust_sleep;
    }
    else {
        duration = 0;
    }

    ztimer_set(clock, &timer, duration);
    mutex_lock(&mutex);
}

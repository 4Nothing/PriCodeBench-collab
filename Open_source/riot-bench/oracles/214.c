int isrpipe_read_timeout(isrpipe_t *isrpipe, uint8_t *buffer, size_t count, uint32_t timeout)
{
    int res = tsrb_get(&isrpipe->tsrb, buffer, count);
    if (res > 0) {
        return res;
    }

    _isrpipe_timeout_t _timeout = { .mutex = &isrpipe->mutex, .flag = 0 };
    ztimer_t timer = { .callback = _cb, .arg = &_timeout };

    ztimer_set(ZTIMER_USEC, &timer, timeout);
    while ((res = tsrb_get(&isrpipe->tsrb, buffer, count)) == 0) {
        mutex_lock(&isrpipe->mutex);
        if (_timeout.flag) {
            /* timer was consumed */
            return -ETIMEDOUT;
        }
    }

    ztimer_remove(ZTIMER_USEC, &timer);
    return res;
}
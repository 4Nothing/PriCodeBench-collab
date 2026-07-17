int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    /* disable alarm to prevent race condition */
    rtt_clear_alarm();

    uint32_t now = rtt_get_counter();

    rtc_tm_normalize(time);

    alarm_time   = rtc_mktime(time);
    alarm_cb_arg = arg;
    alarm_cb     = cb;

    /* RTT interrupt is disabled here */
    rtc_now      = _rtc_now(now);
    _update_alarm(now);

    return 0;
}
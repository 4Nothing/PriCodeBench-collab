int rtc_set_time(struct tm *time)
{
    rtc_tm_normalize(time);

    /* disable alarm to prevent race condition */
    rtt_clear_alarm();

    uint32_t now = rtt_get_counter();
    rtc_now      = rtc_mktime(time);

    /* calculate next wake-up period */
    _update_alarm(now);

    return 0;
}
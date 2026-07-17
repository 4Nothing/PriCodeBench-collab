void rtt_rtc_settimeofday(uint32_t s, uint32_t us)
{
    /* disable alarm to prevent race condition */
    rtt_clear_alarm();
    uint32_t now = ((uint64_t)us * RTT_SECOND) / US_PER_SEC;
    rtc_now      = s;
    rtt_set_counter(now);
    /* calculate next wake-up period */
    _update_alarm(0);
}
void rtc_init(void)
{
    /* only update last_alarm on cold boot */
    if (last_alarm == 0) {
        last_alarm = rtt_get_counter();
    }

    _set_alarm(last_alarm, TICKS(RTT_SECOND_MAX));
}
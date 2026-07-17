int rtc_get_time(struct tm *time)
{
    uint32_t now, tmp;
    unsigned state = irq_disable();

    now  = rtt_get_counter();
    tmp  = _rtc_now(now);

    irq_restore(state);
    rtc_localtime(tmp, time);

    return 0;
}
void rtt_rtc_gettimeofday(uint32_t *s, uint32_t *us)
{
    uint32_t now;
    unsigned state = irq_disable();

    now  = rtt_get_counter();
    *s   = _rtc_now(now);
    *us  = ((uint64_t)SUBSECONDS(now - last_alarm) * US_PER_SEC)
         / RTT_SECOND;

    irq_restore(state);
}
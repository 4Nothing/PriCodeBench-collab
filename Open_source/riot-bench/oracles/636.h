static inline uint32_t xtimer_now_usec(void)
{
    return xtimer_usec_from_ticks(xtimer_now());
}

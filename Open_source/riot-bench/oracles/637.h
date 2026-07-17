static inline uint64_t xtimer_now_usec64(void)
{
    return xtimer_usec_from_ticks64(xtimer_now64());
}

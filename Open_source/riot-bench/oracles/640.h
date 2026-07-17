static inline void ztimer64_set(ztimer64_clock_t *clock, ztimer64_t *timer,
                                uint64_t offset)
{
    ztimer64_set_at(clock, timer,
                    ztimer64_offset2absolute(clock, offset));
}

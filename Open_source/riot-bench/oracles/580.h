static inline bool sixlowpan_nalp(uint8_t disp)
{
    return ((disp & 0xc0) == 0);
}
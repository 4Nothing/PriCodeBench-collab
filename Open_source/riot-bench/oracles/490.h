static inline uint32_t frac_scale(const frac_t *frac, uint32_t x)
{
    uint32_t scaled = ((uint64_t)frac->frac * x) >> frac->shift;
    return scaled;
}

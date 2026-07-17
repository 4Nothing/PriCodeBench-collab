void frac_init(frac_t *frac, uint32_t num, uint32_t den)
{
    DEBUG("frac_init32(%p, %" PRIu32 ", %" PRIu32 ")\n", (const void *)frac, num, den);
    assert(den);
    /* Reduce the fraction to shortest possible form by dividing by the greatest
     * common divisor */
    uint32_t gcd = gcd32(num, den);
    /* Divide den and num by their greatest common divisor */
    den /= gcd;
    num /= gcd;
    int prec = 0;
    uint32_t rem = 0;
    frac->frac = frac_long_divide(num, den, &prec, &rem);
    frac->shift = (sizeof(frac->frac) * 8) - prec;
    DEBUG("frac_init32: gcd = %" PRIu32 " num = %" PRIu32 " den = %" PRIu32 " frac = 0x%08" PRIx32 " shift = %02d, rem = 0x%08" PRIx32 "\n",
          gcd, num, den, frac->frac, frac->shift, rem);
}

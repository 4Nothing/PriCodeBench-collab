int32_t adc_util_map(int sample, adc_res_t res, int32_t min, int32_t max)
{
    /* Using 64 bit signed int as intermediate to prevent overflow when range
     * multiplied by sample requires more than 32 bits */
    int32_t scaled = (((int64_t)(max - min) * sample) >> _adc_res_bits(res));
    DEBUG("scaled: %" PRId32 "\n", scaled);
    return (min + scaled);
}


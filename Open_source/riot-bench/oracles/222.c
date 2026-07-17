void matstat_merge(matstat_state_t *dest, const matstat_state_t *src)
{
    if (src->count == 0) {
        /* src is empty, no-op */
        return;
    }
    if (dest->count == 0) {
        /* dest is empty, straight copy */
        *dest = *src;
        return;
    }
    /* Combining the variance of the two samples needs some extra
     * handling if the means are different between the two states,
     * source: https://stats.stackexchange.com/a/43183
     * (using sum_sq = sigma2 * n, instead of sum_sq = sigma2 * (n-1) to simplify algorithm)
     */
    dest->sum_sq = (dest->sum_sq + dest->sum * dest->mean + src->sum_sq + src->sum * src->mean);
    dest->count += src->count;
    dest->sum += src->sum;
    int32_t new_mean = dest->sum / dest->count;
    int64_t diff = -new_mean * dest->sum;
    if ((diff < 0) && ((uint64_t)(-diff) > dest->sum_sq)) {
        /* Handle corner cases where sum_sq becomes negative */
        dest->sum_sq = 0;
    }
    else {
        dest->sum_sq += diff;
    }
    dest->mean = new_mean;
    if (src->max > dest->max) {
        dest->max = src->max;
    }
    if (src->min < dest->min) {
        dest->min = src->min;
    }
}
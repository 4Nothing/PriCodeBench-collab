void matstat_add(matstat_state_t *state, int32_t value)
{
    if (value > state->max) {
        state->max = value;
    }
    if (value < state->min) {
        state->min = value;
    }
    state->sum += value;
    /* Using Welford's algorithm for variance */
    ++state->count;
    if (state->count == 1) {
        state->sum_sq = 0;
        state->mean = value;
    }
    else {
        int32_t new_mean = state->sum / state->count;
        int64_t diff = (value - state->mean) * (value - new_mean);
        if ((diff < 0) && ((uint64_t)(-diff) > state->sum_sq)) {
            /* Handle corner cases where sum_sq becomes negative */
            state->sum_sq = 0;
        }
        else {
            state->sum_sq += diff;
        }
        state->mean = new_mean;
    }
}
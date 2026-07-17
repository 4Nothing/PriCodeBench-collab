uint64_t matstat_variance(const matstat_state_t *state)
{
    if (state->count < 2) {
        /* We don't have any way of returning an error */
        return 0;
    }
    uint64_t variance = state->sum_sq / (state->count - 1);
#ifdef PRIu64
    DEBUG("Var: (%" PRIu64 " / (%" PRId32 " - 1)) = %" PRIu64 "\n",
        state->sum_sq, state->count, variance);
#else
    DEBUG("Var: (%" PRIu32 "%s / (%" PRId32 " - 1)) = %" PRIu32 "%s\n",
          (uint32_t)state->sum_sq, (state->sum_sq > UINT32_MAX) ? "!trunc " : "",
          state->count,
          (uint32_t)variance, (variance > UINT32_MAX) ? "!trunc " : "");
#endif
    return variance;
}
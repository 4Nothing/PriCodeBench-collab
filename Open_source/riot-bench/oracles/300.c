timex_t timex_set(uint32_t seconds, uint32_t microseconds)
{
    timex_t result;
    result.seconds = seconds;
    result.microseconds = microseconds;

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if (!timex_isnormalized(&result)) {
            puts("timex_set on denormalized value");
        }
    }

    return result;
}
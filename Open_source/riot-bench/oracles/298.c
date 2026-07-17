timex_t timex_add(const timex_t a, const timex_t b)
{
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if (!timex_isnormalized(&a) || !timex_isnormalized(&b)) {
            puts("timex_add on denormalized value");
        }
    }

    timex_t result;
    result.seconds = a.seconds + b.seconds;
    result.microseconds = a.microseconds + b.microseconds;

    if (result.microseconds > US_PER_SEC) {
        result.microseconds -= US_PER_SEC;
        result.seconds++;
    }

    return result;
}
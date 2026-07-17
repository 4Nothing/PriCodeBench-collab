timex_t timex_sub(const timex_t a, const timex_t b)
{
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if (!timex_isnormalized(&a) || !timex_isnormalized(&b)) {
            puts("timex_sub on denormalized value");
        }
    }

    timex_t result;

    if (a.microseconds >= b.microseconds) {
        result.seconds = a.seconds - b.seconds;
        result.microseconds = a.microseconds - b.microseconds;
    }
    else {
        result.seconds = a.seconds - b.seconds - 1;
        result.microseconds = a.microseconds + US_PER_SEC - b.microseconds;
    }

    return result;
}
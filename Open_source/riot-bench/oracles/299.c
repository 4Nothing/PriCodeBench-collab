int timex_cmp(const timex_t a, const timex_t b)
{
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if (!timex_isnormalized(&a) || !timex_isnormalized(&b)) {
            puts("timex_cmp on denormalized value");
        }
    }

    if (a.seconds < b.seconds) {
        return -1;
    }

    if (a.seconds == b.seconds) {
        if (a.microseconds < b.microseconds) {
            return -1;
        }

        if (a.microseconds == b.microseconds) {
            return 0;
        }
    }

    return 1;
}
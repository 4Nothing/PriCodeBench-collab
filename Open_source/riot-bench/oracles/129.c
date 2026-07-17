int fmt_is_number(const char *str)
{
    if (!str || !*str) {
        return 0;
    }
    for (; *str; str++) {
        if (!fmt_is_digit(*str)) {
            return 0;
        }
    }

    return 1;
}
int rtc_tm_compare(const struct tm *a, const struct tm *b)
{
    RETURN_IF_DIFFERENT(a, b, tm_year);
    RETURN_IF_DIFFERENT(a, b, tm_mon);
    RETURN_IF_DIFFERENT(a, b, tm_mday);
    RETURN_IF_DIFFERENT(a, b, tm_hour);
    RETURN_IF_DIFFERENT(a, b, tm_min);
    RETURN_IF_DIFFERENT(a, b, tm_sec);

    return 0;
}
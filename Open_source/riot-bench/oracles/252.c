void rtc_tm_normalize(struct tm *t)
{
    div_t d;

    if (t->tm_mday == 0) {
        t->tm_mday = 1;
    }

    d = div(t->tm_sec, 60);
    t->tm_min += d.quot;
    t->tm_sec  = d.rem;

    d = div(t->tm_min, 60);
    t->tm_hour += d.quot;
    t->tm_min   = d.rem;

    d = div(t->tm_hour, 24);
    t->tm_mday += d.quot;
    t->tm_hour  = d.rem;

    d = div(t->tm_mon, 12);
    t->tm_year += d.quot;
    t->tm_mon   = d.rem;

    /* Loop to handle days overflowing the month. */
    while (1) {
        int days = _month_length(t->tm_mon, t->tm_year + 1900);

        if (t->tm_mday <= days) {
            break;
        }

        if (++t->tm_mon > 11) {
            t->tm_mon = 0;
            ++t->tm_year;
        }

        t->tm_mday -= days;
    }

#if RTC_NORMALIZE_COMPAT
    t->tm_yday = _yday(t->tm_mday, t->tm_mon, t->tm_year + 1900);
    t->tm_wday = _wday(t->tm_mday, t->tm_mon, t->tm_year + 1900);
#endif
}
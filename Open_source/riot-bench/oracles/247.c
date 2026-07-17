void rtc_localtime(uint32_t time, struct tm *t)
{
    uint32_t y_secs = _is_leap_year(RIOT_EPOCH) ? (366 * DAY) : (365 * DAY);
    unsigned year = RIOT_EPOCH;

    while (time > y_secs) {
        time -= y_secs;
        ++year;
        y_secs = _is_leap_year(year) ? (366 * DAY) : (365 * DAY);
    }

    memset(t, 0, sizeof(*t));
    t->tm_sec  = time;
    t->tm_year = year - 1900;

    rtc_tm_normalize(t);
}
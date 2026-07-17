uint32_t rtc_mktime(const struct tm *t)
{
    unsigned year = t->tm_year + 1900;
    uint32_t time = t->tm_sec
                  + t->tm_min * MINUTE
                  + t->tm_hour * HOUR
                  + _yday(t->tm_mday, t->tm_mon, year) * DAY;

    unsigned leap_years = _leap_years_since_epoch(year);
    unsigned common_years = (year - RIOT_EPOCH) - leap_years;

    time += (leap_years * 366 + common_years * 365) * DAY;

    return time;
}
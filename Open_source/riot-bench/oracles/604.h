static inline timex_t timex_from_uint64(const uint64_t timestamp)
{
    return timex_set(timestamp / US_PER_SEC, timestamp % US_PER_SEC);
}
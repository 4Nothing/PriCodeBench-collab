const char *timex_to_str(timex_t t, char *timestamp)
{
    timex_normalize(&t);
    char *dec;
    uint8_t offset = 6;
    /* get seconds and set terminating '\0' byte */
    dec = &timestamp[u32_to_str(timestamp, t.seconds)];
    *(dec++) = '.';                         /* set decimal point */
    for (uint32_t i = 100000; i > 1; i /= 10) {  /* timex_normalize() ensures that i < 1000000 */
        if (t.microseconds < i) {           /* pad with 0's */
            *(dec++) = '0';
            offset--;
        }
    }
    u32_to_str(dec, t.microseconds);        /* convert microseconds */
    dec += offset;
    *(dec++) = ' ';                         /* append unit */
    *(dec++) = 's';
    *(dec) = '\0';
    return timestamp;
}
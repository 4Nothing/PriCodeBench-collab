ssize_t strscpy(char *dest, const char *src, size_t count)
{
    const char *start = dest;

    if (!count) {
        return -E2BIG;
    }

    while (--count && *src) {
        *dest++ = *src++;
    }

    *dest = 0;

    if (*src == 0) {
        return dest - start;
    } else {
        return -E2BIG;
    }
}
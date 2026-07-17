size_t fmt_str(char *out, const char *str)
{
    int len = 0;

    if (!out) {
        len = fmt_strlen(str);
    }
    else {
        char c;
        while ((c = *str++)) {
            *out++ = c;
            len++;
        }
    }
    return len;
}
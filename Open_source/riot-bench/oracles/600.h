static inline size_t string_writer_len(const string_writer_t *sw)
{
    return sw->position - sw->start;
}

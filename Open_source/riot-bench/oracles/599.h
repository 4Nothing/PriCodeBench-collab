static inline void string_writer_init(string_writer_t *sw, void *buffer, size_t len)
{
    assert(buffer && len);

    sw->start = buffer;
    sw->position = buffer;
    sw->capacity = len;
    sw->position[0] = 0;
}

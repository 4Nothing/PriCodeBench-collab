static inline unsigned int cib_avail(const cib_t *cib)
{
    return cib->write_count - cib->read_count;
}

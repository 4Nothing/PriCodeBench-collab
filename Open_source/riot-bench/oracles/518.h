static inline bool ipv4_addr_equal(const ipv4_addr_t *a, const ipv4_addr_t *b)
{
    return (a->u32.u32 == b->u32.u32);
}
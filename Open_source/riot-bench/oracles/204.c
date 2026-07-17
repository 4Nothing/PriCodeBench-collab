bool ipv6_addr_equal(const ipv6_addr_t *a, const ipv6_addr_t *b)
{
    return (a->u64[0].u64 == b->u64[0].u64) &&
           (a->u64[1].u64 == b->u64[1].u64);
}
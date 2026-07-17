int fib_devel_get_lifetime(fib_table_t *table, uint64_t *lifetime, uint8_t *dst,
                           size_t dst_size)
{
    if (table->table_type == FIB_TABLE_TYPE_SH) {
        size_t count = 1;
        fib_entry_t *entry[count];

        int ret = fib_find_entry(table, dst, dst_size, &(entry[0]), &count);
        if (ret == 1 ) {
            /* only return lifetime of exact matches */
            *lifetime = entry[0]->lifetime;
            return 0;
        }
        return -EHOSTUNREACH;
    }
    else if (table->table_type == FIB_TABLE_TYPE_SR) {
        size_t addr_size_match = dst_size << 3;
        /* first hit wins here */
        for (size_t i = 0; i < table->size; ++i) {
            if (universal_address_compare(table->data.source_routes->headers[i].sr_dest->address,
                                        dst, &addr_size_match) == UNIVERSAL_ADDRESS_EQUAL) {
                *lifetime = table->data.source_routes->headers[i].sr_lifetime;
                return 0;
            }
        }
        return -EHOSTUNREACH;
    }
    return -EFAULT;
}
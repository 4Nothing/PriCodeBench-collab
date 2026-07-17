int fib_get_destination_set(fib_table_t *table, uint8_t *prefix,
                            size_t prefix_size,
                            fib_destination_set_entry_t *dst_set,
                            size_t* dst_set_size)
{
    mutex_lock(&(table->mtx_access));
    int ret = -EHOSTUNREACH;
    size_t found_entries = 0;

    for (size_t i = 0; i < table->size; ++i) {
        fib_entry_t *tmp = &table->data.entries[i];
        if ((tmp->global != NULL)
            && (UNIVERSAL_ADDRESS_EQUAL <= universal_address_compare_prefix(tmp->global, prefix,
                                                                            prefix_size <<3 ))) {
            if ((dst_set != NULL) && (found_entries < *dst_set_size) ) {
            /* set the size to full byte usage */
            dst_set[found_entries].dest_size = sizeof(dst_set[found_entries].dest);
            universal_address_get_address(table->data.entries[i].global,
                                          dst_set[found_entries].dest,
                                          &dst_set[found_entries].dest_size);
            }
            found_entries++;
        }
    }

    if (found_entries > *dst_set_size) {
        ret = -ENOBUFS;
    }
    else if (found_entries > 0) {
        ret = 0;
    }

    *dst_set_size = found_entries;

    mutex_unlock(&(table->mtx_access));

    return ret;
}
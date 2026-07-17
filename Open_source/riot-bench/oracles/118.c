int fib_update_entry(fib_table_t *table, uint8_t *dst, size_t dst_size,
                     uint8_t *next_hop, size_t next_hop_size,
                     uint32_t next_hop_flags, uint32_t lifetime)
{
    mutex_lock(&(table->mtx_access));
    DEBUG("[fib_update_entry]\n");
    size_t count = 1;
    fib_entry_t *entry[count];
    int ret = -ENOMEM;

    /* check if dst and next_hop are valid pointers */
    if ((dst == NULL) || (next_hop == NULL)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_find_entry(table, dst, dst_size, &(entry[0]), &count) == 1) {
        DEBUG("[fib_update_entry] found entry: %p\n", (void *)(entry[0]));
        /* we must take the according entry and update the values */
        ret = fib_upd_entry(entry[0], next_hop, next_hop_size, next_hop_flags, lifetime);
    }
    else {
        /* we have ambiguous entries, i.e. count > 1
         * this should never happen
         */
        DEBUG("[fib_update_entry] ambiguous entries detected!!!\n");
    }

    mutex_unlock(&(table->mtx_access));
    return ret;
}
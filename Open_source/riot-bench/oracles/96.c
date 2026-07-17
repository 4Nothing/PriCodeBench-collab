int fib_add_entry(fib_table_t *table,
                  kernel_pid_t iface_id, uint8_t *dst, size_t dst_size,
                  uint32_t dst_flags, uint8_t *next_hop, size_t next_hop_size,
                  uint32_t next_hop_flags, uint32_t lifetime)
{
    mutex_lock(&(table->mtx_access));
    DEBUG("[fib_add_entry]\n");
    size_t count = 1;
    fib_entry_t *entry[count];

    /* check if dst and next_hop are valid pointers */
    if ((dst == NULL) || (next_hop == NULL)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    int ret = fib_find_entry(table, dst, dst_size, &(entry[0]), &count);

    if (ret == 1) {
        /* we must take the according entry and update the values */
        ret = fib_upd_entry(entry[0], next_hop, next_hop_size, next_hop_flags, lifetime);
    }
    else {
        ret = fib_create_entry(table, iface_id, dst, dst_size, dst_flags,
                               next_hop, next_hop_size, next_hop_flags, lifetime);
    }

    mutex_unlock(&(table->mtx_access));
    return ret;
}
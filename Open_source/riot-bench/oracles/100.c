int fib_get_next_hop(fib_table_t *table, kernel_pid_t *iface_id,
                     uint8_t *next_hop, size_t *next_hop_size,
                     uint32_t *next_hop_flags, uint8_t *dst, size_t dst_size,
                     uint32_t dst_flags)
{
    mutex_lock(&(table->mtx_access));
    DEBUG("[fib_get_next_hop]\n");
    size_t count = 1;
    fib_entry_t *entry[count];

    if ((iface_id == NULL)
        || (next_hop_size == NULL)
        || (next_hop_flags == NULL)) {
            mutex_unlock(&(table->mtx_access));
            return -EINVAL;
        }

    if ((dst == NULL) || (next_hop == NULL)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    int ret = fib_find_entry(table, dst, dst_size, &(entry[0]), &count);
    if (!(ret == 0 || ret == 1)) {
        /* notify all responsible RPs for unknown  next-hop for the destination address */
        if (fib_signal_rp(table, FIB_MSG_RP_SIGNAL_UNREACHABLE_DESTINATION,
                          dst, dst_size, dst_flags) == 0) {
            count = 1;
            /* now lets see if the RRPs have found a valid next-hop */
            ret = fib_find_entry(table, dst, dst_size, &(entry[0]), &count);
        }
    }

    if (ret == 0 || ret == 1) {

        uint8_t *address_ret = universal_address_get_address(entry[0]->next_hop,
                               next_hop, next_hop_size);

        if (address_ret == NULL) {
            mutex_unlock(&(table->mtx_access));
            return -ENOBUFS;
        }
    }
    else {
        mutex_unlock(&(table->mtx_access));
        return -EHOSTUNREACH;
    }

    *iface_id = entry[0]->iface_id;
    *next_hop_flags = entry[0]->next_hop_flags;
    mutex_unlock(&(table->mtx_access));
    return 0;
}
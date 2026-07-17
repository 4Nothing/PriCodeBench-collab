int fib_sr_entry_append(fib_table_t *table, fib_sr_t *fib_sr,
                        uint8_t *addr, size_t addr_size)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (addr == NULL)
        || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    fib_sr_entry_t *elt = NULL;
    LL_FOREACH(fib_sr->sr_path, elt) {
        size_t addr_size_match = addr_size << 3;
        if (UNIVERSAL_ADDRESS_EQUAL == universal_address_compare(elt->address, addr,
                                                                 &addr_size_match)) {
            mutex_unlock(&(table->mtx_access));
            return -EINVAL;
        }
    }

    fib_sr_entry_t *new_entry[1];
    int ret = fib_sr_new_entry(table, addr, addr_size, &new_entry[0]);

    if (ret == 0) {
        fib_sr_entry_t *tmp = fib_sr->sr_dest;
        if (tmp != NULL) {
            /* we append the new entry behind the former destination */
            tmp->next = new_entry[0];
        }
        else {
            /* this is also our first entry */
            fib_sr->sr_path = new_entry[0];
        }
        fib_sr->sr_dest = new_entry[0];
    }

    mutex_unlock(&(table->mtx_access));
    return ret;
}
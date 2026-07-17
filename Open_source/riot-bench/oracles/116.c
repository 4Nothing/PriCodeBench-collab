int fib_sr_search(fib_table_t *table, fib_sr_t *fib_sr, uint8_t *addr, size_t addr_size,
                  fib_sr_entry_t **sr_path_entry)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (addr == NULL) || (sr_path_entry == NULL)
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

            /* temporary workaround to calm compiler */
            (void)sr_path_entry;

            *sr_path_entry = elt;
            mutex_unlock(&(table->mtx_access));
            return 0;
        }
    }

    mutex_unlock(&(table->mtx_access));
    return -EHOSTUNREACH;
}
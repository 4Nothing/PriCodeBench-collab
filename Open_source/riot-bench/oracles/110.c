int fib_sr_entry_get_address(fib_table_t *table, fib_sr_t *fib_sr, fib_sr_entry_t *sr_entry,
                             uint8_t *addr, size_t *addr_size)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    fib_sr_entry_t *elt = NULL;
    LL_FOREACH(fib_sr->sr_path, elt) {
        if (elt == sr_entry) {
            if (universal_address_get_address(elt->address, addr, addr_size) != NULL) {
                mutex_unlock(&(table->mtx_access));
                return 0;
            }
            else {
                mutex_unlock(&(table->mtx_access));
                return -ENOMEM;
            }
        }
    }
    mutex_unlock(&(table->mtx_access));
    return -ENOENT;
}
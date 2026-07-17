int fib_sr_entry_overwrite(fib_table_t *table, fib_sr_t *fib_sr,
                           uint8_t *addr_old, size_t addr_old_size,
                           uint8_t *addr_new, size_t addr_new_size)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (addr_old == NULL) || (addr_new == NULL)
        || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    fib_sr_entry_t *elt = NULL, *elt_repl;
    elt_repl = NULL;
    LL_FOREACH(fib_sr->sr_path, elt) {
        size_t addr_old_size_match = addr_old_size << 3;
        size_t addr_new_size_match = addr_old_size_match;
        if (UNIVERSAL_ADDRESS_EQUAL == universal_address_compare(elt->address, addr_old,
                                                                 &addr_old_size_match)) {
            elt_repl = elt;
        }

        if (UNIVERSAL_ADDRESS_EQUAL == universal_address_compare(elt->address, addr_new,
                                                                 &addr_new_size_match)) {
            mutex_unlock(&(table->mtx_access));
            return -EINVAL;
        }
    }

    if (elt_repl != NULL) {
        universal_address_rem(elt_repl->address);
        universal_address_container_t *add = universal_address_add(addr_new, addr_new_size);

        if (add == NULL) {
            /* if this happened we deleted one entry, i.e. decreased the usecount
             * adding a new one was not possible since lack of memory
             * so we add back the old entry, i.e. increasing the usecount
             */
            universal_address_add(addr_old, addr_old_size);
            mutex_unlock(&(table->mtx_access));
            return -ENOMEM;
        }
        elt_repl->address = add;
    }

    mutex_unlock(&(table->mtx_access));
    return 0;
}
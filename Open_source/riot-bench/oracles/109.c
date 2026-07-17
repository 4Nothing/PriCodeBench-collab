int fib_sr_entry_delete(fib_table_t *table, fib_sr_t *fib_sr, uint8_t *addr, size_t addr_size,
                        bool keep_remaining_route)
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

    fib_sr_entry_t *elt = NULL, *tmp;
    tmp = fib_sr->sr_path;
    LL_FOREACH(fib_sr->sr_path, elt) {
        size_t addr_size_match = addr_size << 3;

        if (UNIVERSAL_ADDRESS_EQUAL == universal_address_compare(elt->address, addr,
                                                                 &addr_size_match)) {
            universal_address_rem(elt->address);
            if (keep_remaining_route) {
                tmp->next = elt->next;
            }
            else {
                fib_sr_entry_t *elt_del = NULL, *tmp_del = NULL;
                LL_FOREACH_SAFE(tmp, elt_del, tmp_del) {
                    universal_address_rem(elt_del->address);
                    elt_del->address = NULL;
                    LL_DELETE(tmp, elt_del);
                }
            }
            if (elt == fib_sr->sr_path) {
                /* if we remove the first entry we must adjust the path start */
                fib_sr->sr_path = elt->next;
            }
            if (elt == fib_sr->sr_dest) {
                /* if we remove the last entry we must adjust the destination */
                fib_sr->sr_dest = tmp;
            }
            mutex_unlock(&(table->mtx_access));
            return 0;
        }
        tmp = elt;
    }

    return -ENOENT;
}
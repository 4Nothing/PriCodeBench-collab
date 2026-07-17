int fib_sr_entry_add(fib_table_t *table, fib_sr_t *fib_sr,
                     fib_sr_entry_t *sr_path_entry, uint8_t *addr, size_t addr_size,
                     bool keep_remaining_route)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (sr_path_entry == NULL) || (addr == NULL)
        || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    bool found = false;
    fib_sr_entry_t *elt = NULL;
    LL_FOREACH(fib_sr->sr_path, elt) {
        size_t addr_size_match = addr_size << 3;
        if (UNIVERSAL_ADDRESS_EQUAL == universal_address_compare(elt->address, addr,
                                                                 &addr_size_match)) {
            mutex_unlock(&(table->mtx_access));
            return -EINVAL;
        }
        if (sr_path_entry == elt) {
            found = true;
            break;
        }
    }

    int ret = -ENOENT;
    if (found) {
        fib_sr_entry_t *new_entry[1];
        ret = fib_sr_new_entry(table, addr, addr_size, &new_entry[0]);
        if (ret == 0) {
            fib_sr_entry_t *remaining = sr_path_entry->next;
            sr_path_entry->next = new_entry[0];
            if (keep_remaining_route) {
                new_entry[0]->next = remaining;
            }
            else {
                fib_sr_entry_t *tmp_elt = NULL, *tmp = NULL;
                LL_FOREACH_SAFE(remaining, tmp_elt, tmp) {
                    universal_address_rem(tmp_elt->address);
                    tmp_elt->address = NULL;
                    LL_DELETE(remaining, tmp_elt);
                }
                new_entry[0]->next = NULL;
                fib_sr->sr_dest = new_entry[0];
            }
        }
    }

    mutex_unlock(&(table->mtx_access));
    return ret;
}
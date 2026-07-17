int fib_sr_next(fib_table_t *table, fib_sr_t *fib_sr, fib_sr_entry_t **sr_path_entry)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (sr_path_entry == NULL)
        || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr->sr_path == NULL) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    /* if we reach the destination entry, i.e. the last entry we just return 1 */
    if (*sr_path_entry == fib_sr->sr_dest) {
        mutex_unlock(&(table->mtx_access));
        return 1;
    }

    /* when we start, we pass the first entry */
    if (*sr_path_entry == NULL) {
        *sr_path_entry = fib_sr->sr_path;
    }
    else {
        /* in any other case we just return the next entry */
        *sr_path_entry = (*sr_path_entry)->next;
    }

    mutex_unlock(&(table->mtx_access));
    return 0;
}
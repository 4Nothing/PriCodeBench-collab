int fib_sr_delete(fib_table_t *table, fib_sr_t *fib_sr)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    fib_sr->sr_lifetime = 0;

    if (fib_sr->sr_path != NULL) {
        fib_sr_entry_t *elt = NULL, *tmp = NULL;
        LL_FOREACH_SAFE(fib_sr->sr_path, elt, tmp) {
            universal_address_rem(elt->address);
            elt->address = NULL;
            LL_DELETE(fib_sr->sr_path, elt);
        }
        fib_sr->sr_path = NULL;
    }

    mutex_unlock(&(table->mtx_access));
    return 0;
}
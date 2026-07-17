void fib_remove_entry(fib_table_t *table, uint8_t *dst, size_t dst_size)
{
    mutex_lock(&(table->mtx_access));
    DEBUG("[fib_remove_entry]\n");
    size_t count = 1;
    fib_entry_t *entry[count];

    int ret = fib_find_entry(table, dst, dst_size, &(entry[0]), &count);

    if (ret == 1) {
        /* we must take the according entry and update the values */
        fib_remove(entry[0]);
    }
    else {
        /* we have ambiguous entries, i.e. count > 1
         * this should never happen
         */
        DEBUG("[fib_update_entry] ambiguous entries detected!!!\n");
    }

    mutex_unlock(&(table->mtx_access));
}
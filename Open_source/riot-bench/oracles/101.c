int fib_get_num_used_entries(fib_table_t *table)
{
    mutex_lock(&(table->mtx_access));
    size_t used_entries = 0;

    for (size_t i = 0; i < table->size; ++i) {
        used_entries += (size_t)(table->data.entries[i].global != NULL);
    }

    mutex_unlock(&(table->mtx_access));
    return used_entries;
}
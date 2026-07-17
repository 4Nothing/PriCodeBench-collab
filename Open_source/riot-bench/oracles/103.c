void fib_print_fib_table(fib_table_t *table)
{
    mutex_lock(&(table->mtx_access));

    for (size_t i = 0; i < table->size; ++i) {
        printf("[fib_print_table] %" PRIuSIZE
               ") iface_id: %d, global: %p, next hop: %p, lifetime: %" PRIu32"\n",
               i, (int)table->data.entries[i].iface_id,
               (void *)table->data.entries[i].global,
               (void *)table->data.entries[i].next_hop,
               (uint32_t)(table->data.entries[i].lifetime / 1000));
    }

    mutex_unlock(&(table->mtx_access));
}
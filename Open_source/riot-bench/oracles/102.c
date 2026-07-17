void fib_init(fib_table_t *table)
{
    DEBUG("[fib_init] hello. Initializing some stuff.\n");
    mutex_init(&(table->mtx_access));
    mutex_lock(&(table->mtx_access));

    for (size_t i = 0; i < FIB_MAX_REGISTERED_RP; ++i) {
        table->notify_rp[i] = KERNEL_PID_UNDEF;
        table->prefix_rp[i] = NULL;
    }

    table->notify_rp_pos = 0;

    if (table->table_type == FIB_TABLE_TYPE_SR) {
        memset(table->data.source_routes->headers, 0,
               sizeof(fib_sr_t) * table->size);
        memset(table->data.source_routes->entry_pool, 0,
               sizeof(fib_sr_entry_t) * table->data.source_routes->entry_pool_size);
    }
    else {
        memset(table->data.entries, 0, (table->size * sizeof(fib_entry_t)));
    }
    universal_address_init();
    mutex_unlock(&(table->mtx_access));
}
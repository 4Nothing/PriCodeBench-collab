int fib_sr_create(fib_table_t *table, fib_sr_t **fib_sr, kernel_pid_t sr_iface_id,
                  uint32_t sr_flags, uint32_t sr_lifetime)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (sr_lifetime == 0)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    for (size_t i = 0; i < table->size; ++i) {
        if (table->data.source_routes->headers[i].sr_lifetime == 0) {
            table->data.source_routes->headers[i].sr_iface_id = sr_iface_id;
            table->data.source_routes->headers[i].sr_flags = sr_flags;
            table->data.source_routes->headers[i].sr_path = NULL;
            table->data.source_routes->headers[i].sr_dest = NULL;
            if (sr_lifetime < (uint32_t)FIB_LIFETIME_NO_EXPIRE) {
                fib_lifetime_to_absolute(sr_lifetime,
                                         &table->data.source_routes->headers[i].sr_lifetime);
            }
            else {
                table->data.source_routes->headers[i].sr_lifetime = FIB_LIFETIME_NO_EXPIRE;
            }
            *fib_sr = &table->data.source_routes->headers[i];
            mutex_unlock(&(table->mtx_access));
            return 0;
        }
    }

    mutex_unlock(&(table->mtx_access));
    return -ENOBUFS;
}
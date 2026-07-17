int fib_sr_read_head(fib_table_t *table, fib_sr_t *fib_sr, kernel_pid_t *iface_id,
                     uint32_t *sr_flags, uint32_t *sr_lifetime)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (iface_id == NULL) || (sr_flags == NULL)
        || (sr_lifetime == NULL) || (fib_is_sr_in_table(table, fib_sr) == -ENOENT) ) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    *iface_id = fib_sr->sr_iface_id;
    *sr_flags = fib_sr->sr_flags;
    *sr_lifetime = fib_sr->sr_lifetime - xtimer_now_usec64();

    mutex_unlock(&(table->mtx_access));
    return 0;
}
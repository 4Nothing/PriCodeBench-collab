int fib_sr_set(fib_table_t *table, fib_sr_t *fib_sr, kernel_pid_t *sr_iface_id,
               uint32_t *sr_flags, uint32_t *sr_lifetime)
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

    if (sr_iface_id != NULL) {
        fib_sr->sr_iface_id = *sr_iface_id;
    }

    if (sr_flags != NULL) {
        fib_sr->sr_flags = *sr_flags;
    }

    if (sr_lifetime != NULL) {
        fib_lifetime_to_absolute(*sr_lifetime, &(fib_sr->sr_lifetime));
    }

    mutex_unlock(&(table->mtx_access));
    return 0;
}
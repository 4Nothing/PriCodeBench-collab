int fib_sr_read_destination(fib_table_t *table, fib_sr_t *fib_sr,
                            uint8_t *dst, size_t *dst_size)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (dst == NULL) || (dst_size == NULL)
        || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    if (fib_sr->sr_dest == NULL) {
        mutex_unlock(&(table->mtx_access));
        return -EHOSTUNREACH;
    }

    if (universal_address_get_address(fib_sr->sr_dest->address, dst, dst_size) == NULL) {
        mutex_unlock(&(table->mtx_access));
        return -ENOBUFS;
    }

    mutex_unlock(&(table->mtx_access));
    return 0;
}
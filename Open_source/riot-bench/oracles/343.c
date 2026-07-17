int universal_address_get_num_used_entries(void)
{
    mutex_lock(&mtx_access);
    size_t ret = universal_address_table_filled;
    mutex_unlock(&mtx_access);
    return ret;
}
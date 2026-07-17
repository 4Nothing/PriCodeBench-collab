size_t nanocoap_cache_used_count(void)
{
    return clist_count(&_cache_list_head);
}


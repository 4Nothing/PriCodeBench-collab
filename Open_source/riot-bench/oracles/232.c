size_t nanocoap_cache_free_count(void)
{
    return clist_count(&_empty_list_head);
}


void nanocoap_cache_init(void)
{
    _cache_list_head.next = NULL;
    _empty_list_head.next = NULL;
    memset(_cache_entries, 0, sizeof(_cache_entries));
    /* construct list of empty entries */
    for (unsigned i = 0; i < CONFIG_NANOCOAP_CACHE_ENTRIES; i++) {
        clist_rpush(&_empty_list_head, &_cache_entries[i].node);
    }
}


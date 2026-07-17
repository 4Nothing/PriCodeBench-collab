static inline void clist_sort(clist_node_t *list, clist_cmp_func_t cmp)
{
    if (list->next) {
        list->next = _clist_sort(list->next->next, cmp);
    }
}

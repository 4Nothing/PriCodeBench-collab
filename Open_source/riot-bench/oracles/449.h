static inline bool clist_is_empty(const clist_node_t *list)
{
    return list->next == NULL;
}

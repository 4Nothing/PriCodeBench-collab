static inline bool clist_exactly_one(clist_node_t *list)
{
    return !clist_is_empty(list) && (list->next == list->next->next);
}

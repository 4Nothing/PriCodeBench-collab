static inline clist_node_t *clist_lpop(clist_node_t *list)
{
    if (list->next) {
        clist_node_t *first = list->next->next;
        if (list->next == first) {
            list->next = NULL;
        }
        else {
            list->next->next = first->next;
        }
        return first;
    }
    else {
        return NULL;
    }
}

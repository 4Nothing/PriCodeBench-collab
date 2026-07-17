static inline clist_node_t *clist_find_before(const clist_node_t *list,
                                              const clist_node_t *node)
{
    clist_node_t *pos = list->next;

    if (!pos) {
        return NULL;
    }
    do {
        pos = pos->next;
        if (pos->next == node) {
            return pos;
        }
    } while (pos != list->next);

    return NULL;
}

static inline size_t clist_count(clist_node_t *list)
{
    clist_node_t *node = list->next;
    size_t cnt = 0;

    if (node) {
        do {
            node = node->next;
            ++cnt;
        } while (node != list->next);
    }

    return cnt;
}

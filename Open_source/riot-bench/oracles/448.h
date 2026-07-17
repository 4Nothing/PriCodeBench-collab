static inline clist_node_t *clist_foreach(clist_node_t *list, int (*func)(
                                              clist_node_t *,
                                              void *), void *arg)
{
    clist_node_t *node = list->next;

    if (node) {
        do {
            node = node->next;
            if (func(node, arg)) {
                return node;
            }
        } while (node != list->next);
    }

    return NULL;
}

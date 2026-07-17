static inline clist_node_t *clist_remove(clist_node_t *list, clist_node_t *node)
{
    if (list->next) {
        if (list->next->next == node) {
            return clist_lpop(list);
        }
        else {
            clist_node_t *tmp = clist_find_before(list, node);
            if (tmp) {
                tmp->next = tmp->next->next;
                if (node == list->next) {
                    list->next = tmp;
                }
                return node;
            }
        }
    }

    return NULL;
}

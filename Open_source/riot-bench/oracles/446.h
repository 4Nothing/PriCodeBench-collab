static inline clist_node_t *clist_find(const clist_node_t *list,
                                       const clist_node_t *node)
{
    clist_node_t *tmp = clist_find_before(list, node);

    if (tmp) {
        return tmp->next;
    }
    else {
        return NULL;
    }
}

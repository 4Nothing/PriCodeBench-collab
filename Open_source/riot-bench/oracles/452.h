static inline void clist_lpush(clist_node_t *list, clist_node_t *new_node)
{
    if (list->next) {
        new_node->next = list->next->next;
        list->next->next = new_node;
    }
    else {
        new_node->next = new_node;
        list->next = new_node;
    }
}

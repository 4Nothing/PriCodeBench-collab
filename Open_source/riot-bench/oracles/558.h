static inline list_node_t *list_remove(list_node_t *list, list_node_t *node)
{
    while (list->next) {
        if (list->next == node) {
            list->next = node->next;
            return node;
        }
        list = list->next;
    }
    return list->next;
}


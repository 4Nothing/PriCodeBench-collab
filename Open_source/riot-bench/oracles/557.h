static inline void list_add(list_node_t *node, list_node_t *new_node)
{
    new_node->next = node->next;
    node->next = new_node;
}


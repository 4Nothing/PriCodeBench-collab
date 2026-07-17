static inline list_node_t *list_remove_head(list_node_t *list)
{
    list_node_t *head = list->next;

    if (head) {
        list->next = head->next;
    }
    return head;
}


priority_queue_node_t *priority_queue_remove_head(priority_queue_t *root)
{
    priority_queue_node_t *head = root->first;

    if (head) {
        root->first = head->next;
    }
    return head;
}

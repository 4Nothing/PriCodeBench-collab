void priority_queue_remove(priority_queue_t *root_, priority_queue_node_t *node)
{
    /* The strict aliasing rules allow this assignment. */
    priority_queue_node_t *root = (priority_queue_node_t *)root_;

    while (root->next != NULL) {
        if (root->next == node) {
            root->next = node->next;
            node->next = NULL;
            return;
        }

        root = root->next;
    }
}

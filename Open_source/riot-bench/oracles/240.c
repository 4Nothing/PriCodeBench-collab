void priority_queue_add(priority_queue_t *root, priority_queue_node_t *new_obj)
{
    /* The strict aliasing rules allow this assignment. */
    priority_queue_node_t *node = (priority_queue_node_t *)root;

    while (node->next != NULL) {
        /* not trying to add the same node twice */
        assert(node->next != new_obj);
        if (node->next->priority > new_obj->priority) {
            new_obj->next = node->next;
            node->next = new_obj;
            return;
        }

        node = node->next;
    }

    node->next = new_obj;
    new_obj->next = NULL;
}

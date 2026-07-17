uint32_t gnrc_priority_pktqueue_length(gnrc_priority_pktqueue_t *queue)
{
    assert(queue != NULL);

    uint32_t length = 0;
    priority_queue_node_t *node = queue->first;
    if (!node) {
        return length;
    }

    length++;
    while (node->next != NULL) {
        length++;
        node = node->next;
    }
    return length;
}
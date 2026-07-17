void gnrc_priority_pktqueue_push(gnrc_priority_pktqueue_t *queue,
                                 gnrc_priority_pktqueue_node_t *node)
{
    assert(queue != NULL);
    assert(node != NULL);
    assert(node->pkt != NULL);
    assert(sizeof(unsigned int) == sizeof(gnrc_pktsnip_t *));

    priority_queue_add(queue, (priority_queue_node_t *)node);
}
void gnrc_priority_pktqueue_flush(gnrc_priority_pktqueue_t *queue)
{
    assert(queue != NULL);

    if (gnrc_priority_pktqueue_length(queue) == 0) {
        return;
    }
    gnrc_priority_pktqueue_node_t *node;
    while ((node = (gnrc_priority_pktqueue_node_t *)priority_queue_remove_head(queue))) {
        gnrc_pktbuf_release(node->pkt);
        _free_node(node);
    }
}
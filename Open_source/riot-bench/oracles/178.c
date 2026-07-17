gnrc_pktsnip_t *gnrc_priority_pktqueue_pop(gnrc_priority_pktqueue_t *queue)
{
    if (!queue || (gnrc_priority_pktqueue_length(queue) == 0)) {
        return NULL;
    }
    priority_queue_node_t *head = priority_queue_remove_head(queue);
    gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *) head->data;
    _free_node((gnrc_priority_pktqueue_node_t *)head);
    return pkt;
}
gnrc_pktsnip_t *gnrc_priority_pktqueue_head(gnrc_priority_pktqueue_t *queue)
{
    if (!queue || (gnrc_priority_pktqueue_length(queue) == 0)) {
        return NULL;
    }
    return (gnrc_pktsnip_t *)queue->first->data;
}
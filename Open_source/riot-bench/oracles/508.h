static inline gnrc_pktqueue_t *gnrc_pktqueue_remove_head(gnrc_pktqueue_t **queue)
{
    return gnrc_pktqueue_remove(queue, *queue);
}
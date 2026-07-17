static inline void gnrc_pktqueue_add(gnrc_pktqueue_t **queue, gnrc_pktqueue_t *node)
{
    LL_APPEND(*queue, node);
}
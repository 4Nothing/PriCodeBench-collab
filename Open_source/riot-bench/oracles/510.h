static inline void gnrc_priority_pktqueue_node_init(gnrc_priority_pktqueue_node_t *node,
                                                    uint32_t priority,
                                                    gnrc_pktsnip_t *pkt)
{
    node->next = NULL;
    node->priority = priority;
    node->pkt = pkt;
}
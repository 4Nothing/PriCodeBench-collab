static inline void gnrc_priority_pktqueue_init(gnrc_priority_pktqueue_t *queue)
{
    gnrc_priority_pktqueue_t qn = PRIORITY_PKTQUEUE_INIT;

    *queue = qn;
}
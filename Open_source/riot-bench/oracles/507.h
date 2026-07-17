static inline gnrc_pktqueue_t *gnrc_pktqueue_remove(gnrc_pktqueue_t **queue, gnrc_pktqueue_t *node)
{
    if (node) {
        LL_DELETE(*queue, node);
        node->next = NULL;
    }

    return node;
}
static inline void priority_queue_node_init(
    priority_queue_node_t *priority_queue_node)
{
    priority_queue_node_t qn = PRIORITY_QUEUE_NODE_INIT;

    *priority_queue_node = qn;
}

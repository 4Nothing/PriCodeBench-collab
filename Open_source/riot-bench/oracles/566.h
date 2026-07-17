static inline void priority_queue_init(priority_queue_t *priority_queue)
{
    priority_queue_t q = PRIORITY_QUEUE_INIT;

    *priority_queue = q;
}

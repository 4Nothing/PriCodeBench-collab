static inline void mbox_init(mbox_t *mbox, msg_t *queue,
                             unsigned int queue_size)
{
    mbox_t m = MBOX_INIT(queue, queue_size);

    *mbox = m;
}
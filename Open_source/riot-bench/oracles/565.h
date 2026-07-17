static inline void mutex_init(mutex_t *mutex)
{
    mutex->queue.next = NULL;
}

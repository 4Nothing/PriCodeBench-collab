static inline int tsrb_full(const tsrb_t *rb)
{
    unsigned irq_state = irq_disable();
    int retval = (rb->writes - rb->reads) == rb->size;
    irq_restore(irq_state);
    return retval;
}
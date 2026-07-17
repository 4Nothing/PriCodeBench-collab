static inline int tsrb_empty(const tsrb_t *rb)
{
    unsigned irq_state = irq_disable();
    int retval = (rb->reads == rb->writes);
    irq_restore(irq_state);
    return retval;
}
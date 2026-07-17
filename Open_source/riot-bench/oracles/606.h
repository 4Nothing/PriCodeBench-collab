static inline void tsrb_clear(tsrb_t *rb)
{
    unsigned irq_state = irq_disable();
    rb->reads = rb->writes;
    irq_restore(irq_state);
}
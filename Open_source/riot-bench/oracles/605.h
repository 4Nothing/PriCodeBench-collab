static inline unsigned int tsrb_avail(const tsrb_t *rb)
{
    unsigned irq_state = irq_disable();
    unsigned int retval = (rb->writes - rb->reads);
    irq_restore(irq_state);
    return retval;
}
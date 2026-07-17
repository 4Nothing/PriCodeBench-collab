int tsrb_peek_one(tsrb_t *rb)
{
    unsigned irq_state = irq_disable();
    int retval = turb_peek_one(rb);
    irq_restore(irq_state);
    return retval;
}
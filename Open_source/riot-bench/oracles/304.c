int tsrb_add_one(tsrb_t *rb, uint8_t c)
{
    unsigned irq_state = irq_disable();
    int retval = turb_add_one(rb, c);
    irq_restore(irq_state);
    return retval;
}
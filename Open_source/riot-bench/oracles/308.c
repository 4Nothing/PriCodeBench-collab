int tsrb_peek(tsrb_t *rb, uint8_t *dst, size_t n)
{
    unsigned irq_state = irq_disable();
    int idx = turb_peek(rb, dst, n);
    irq_restore(irq_state);
    return idx;
}
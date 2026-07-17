int tsrb_get(tsrb_t *rb, uint8_t *dst, size_t n)
{
    unsigned irq_state = irq_disable();
    int cnt = turb_get(rb, dst, n);
    irq_restore(irq_state);
    return cnt;
}
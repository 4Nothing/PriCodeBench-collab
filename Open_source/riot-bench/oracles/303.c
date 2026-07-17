int tsrb_add(tsrb_t *rb, const uint8_t *src, size_t n)
{
    unsigned irq_state = irq_disable();
    int cnt = turb_add(rb, src, n);
    irq_restore(irq_state);
    return cnt;
}
int tsrb_drop(tsrb_t *rb, size_t n)
{
    unsigned irq_state = irq_disable();
    int cnt = turb_drop(rb, n);
    irq_restore(irq_state);
    return cnt;
}
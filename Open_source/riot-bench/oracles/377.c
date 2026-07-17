uint64_t ztimer64_now(ztimer64_clock_t *clock)
{
    uint64_t now;
    unsigned state = irq_disable();
    uint32_t base_now = ztimer_now(clock->base_clock);

    /* ztimer64 checkpointing works by storing
     * the upper 33 bits in clock->checkpoint.
     * The final time is the lower 32bit time ORed
     * with that checkpoint.
     * The highest bit of the lower 32bit is used
     * as control bit. If the checkpoint and 32bit
     * time both have that bit set (or not), no correction is necessary.
     * If checkpoint and 32bit time disagree on that bit,
     * the checkpoint lags behind, and needs to be updated.
     * This works as long as the checkpoint is at most (2**32-1) behind,
     * which is ensured by setting the base clock timer at most
     * ZTIMER64_CHECKPOINT_INTERVAL into the future.
     */
    if ((clock->checkpoint & ZTIMER64_CHECKPOINT_INTERVAL)
        ^ (base_now & ZTIMER64_CHECKPOINT_INTERVAL)) {
        clock->checkpoint += ZTIMER64_CHECKPOINT_INTERVAL;
    }

    now = clock->checkpoint | base_now;

    irq_restore(state);
    return now;
}

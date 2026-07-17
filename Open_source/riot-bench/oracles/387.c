bool ztimer_release(ztimer_clock_t *clock)
{
    bool no_clock_user_left = false;
    unsigned state = irq_disable();

    assert(clock->users > 0);

    DEBUG("ztimer_release(): %p: %" PRIu16 " user(s)\n",
          (void *)clock, (uint16_t)(clock->users - 1));

    if (--clock->users == 0) {
        /* make sure the timer isn't armed before turning off */
        clock->ops->cancel(clock);

        if (clock->ops->stop) {
            clock->ops->stop(clock);
        }

        no_clock_user_left = true;
    }

    irq_restore(state);

    return no_clock_user_left;
}

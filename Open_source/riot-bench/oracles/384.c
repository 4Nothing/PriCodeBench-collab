void ztimer_mock_advance(ztimer_mock_t *self, uint32_t val)
{
    DEBUG(
        "zmock_advance: start now=0x%08" PRIx32 " + 0x%08" PRIx32 ", target=0x%08" PRIx32 " (%u)\n",
        self->now, val, self->target, self->armed);
    while (val) {
        uint32_t step =
            self->armed ? (self->target < val ? self->target : val) : val;
        DEBUG(
            "zmock_advance: step now=0x%08" PRIx32 " + 0x%08" PRIx32 ", target=0x%08" PRIx32 " (%u)\n",
            self->now, step, self->target, self->armed);

        self->now = (self->now + step) & self->mask;
        if (self->armed) {
            /* Update target */
            if (step >= self->target) {
                /* Target was hit */
                ztimer_mock_fire(self);
            }
            else {
                self->target -= step;
            }
        }
        val -= step;
    }
    DEBUG(
        "zmock_advance: done now=0x%08" PRIx32 " + 0x%08" PRIx32 ", target=0x%08" PRIx32 " (%u)\n",
        self->now, val, self->target, self->armed);
}


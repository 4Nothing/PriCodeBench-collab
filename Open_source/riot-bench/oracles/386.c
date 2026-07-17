void ztimer_mock_jump(ztimer_mock_t *self, uint32_t target)
{
    self->now = target & self->mask;
    DEBUG("zmock_jump: now=0x%08" PRIx32 ", target=0x%08" PRIx32 " (%u)\n",
          self->now, self->target, self->armed);
    /* Do not touch target */
}


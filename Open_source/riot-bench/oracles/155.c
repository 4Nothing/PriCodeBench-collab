void gnrc_netreg_acquire_shared(void) {
    mutex_lock(&_lock_for_counter);
    if (_lock_counter == 0) {
        /* At most, this blocks for the very short time until
         * _gnrc_netreg_acquire_exclusive returns it immediately */
        mutex_lock(&_lock_wait_exclusive);
    }
    if (_lock_counter != UINT_MAX) {
        _lock_counter += 1;
    }
    mutex_unlock(&_lock_for_counter);
}
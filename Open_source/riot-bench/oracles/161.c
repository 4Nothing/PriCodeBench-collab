void gnrc_netreg_release_shared(void) {
    mutex_lock(&_lock_for_counter);

    assert(_lock_counter != 0); /* Release without acquire */

    if (_lock_counter != UINT_MAX) {
        _lock_counter -= 1;
    }
    if (_lock_counter == 0) {
        mutex_unlock(&_lock_wait_exclusive);
    }
    mutex_unlock(&_lock_for_counter);
}
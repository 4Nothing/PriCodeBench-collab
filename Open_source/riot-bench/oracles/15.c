int bf_get_unset(uint8_t field[], size_t size)
{
    int result = -1;
    unsigned nbytes = (size + 7) / 8;

    unsigned state = irq_disable();

    /* skip full bytes */
    unsigned i = _skip_bytes(field, nbytes, 0xff);

    for (; i < size; i++) {
        if (!bf_isset(field, i)) {
            bf_set(field, i);
            result = i;
            break;
        }
    }

    irq_restore(state);
    return result;
}
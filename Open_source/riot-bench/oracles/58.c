bool coding_xor_recover(void *data, size_t len, uint8_t *parity,
                        uint8_t *bitfield, size_t block_size, bool recover_parity)
{
    size_t num_data_chunks   = len / block_size;
    size_t num_parity_chunks = CODING_XOR_PARITY_LEN(len) / block_size;

    if (!_recover_blocks(data, len, parity, bitfield, block_size)) {
        return false;
    }

    _unmix(data, len);

    if (!recover_parity) {
        return true;
    }

    /* recover lost parity blocks */
    for (size_t i = 0; i < num_parity_chunks; ++i) {
        if (bf_isset(bitfield, num_data_chunks + i)) {
            continue;
        }

        DEBUG("regenerate parity block %" PRIuSIZE "\n", i);
        size_t data_len = block_size * CONFIG_CODING_XOR_CHECK_BYTES;
        _gen_parity((uint8_t *)data + i * data_len,
                              data_len, parity + i * block_size);
    }

    return true;
}
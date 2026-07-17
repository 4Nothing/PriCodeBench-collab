int sha256_chain_verify_element(void *element,
                                size_t element_index,
                                void *tail_element,
                                size_t chain_length)
{
    unsigned char tmp_element[SHA256_DIGEST_LENGTH];

    int delta_count = (chain_length - element_index);

    /* assert if we have an index mismatch */
    assert(delta_count >= 1);

    memcpy((void *)tmp_element, element, SHA256_DIGEST_LENGTH);

    /* perform all consecutive iterations down to tail_element */
    for (int i = 0; i < (delta_count - 1); ++i) {
        sha256_inplace(tmp_element);
    }

    /* return if the computed element equals the tail_element */
    return (memcmp(tmp_element, tail_element, SHA256_DIGEST_LENGTH) != 0);
}

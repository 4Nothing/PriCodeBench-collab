void *sha256_chain(const void *seed, size_t seed_length,
                   size_t elements, void *tail_element)
{
    unsigned char tmp_element[SHA256_DIGEST_LENGTH];

    /* assert if no sha256-chain can be created */
    assert(elements >= 2);

    /* 1st iteration */
    sha256(seed, seed_length, tmp_element);

    /* perform consecutive iterations minus the first one */
    for (size_t i = 0; i < (elements - 1); ++i) {
        sha256_inplace(tmp_element);
    }

    /* store the result */
    memcpy(tail_element, tmp_element, SHA256_DIGEST_LENGTH);

    return tail_element;
}

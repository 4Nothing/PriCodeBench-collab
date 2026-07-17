void universal_address_print_table(void)
{
    printf("[universal_address_print_table] universal_address_table_filled: %" PRIuSIZE "\n", \
           universal_address_table_filled);

    /* cppcheck-suppress unsignedLessThanZero
     * (reason: UNIVERSAL_ADDRESS_MAX_ENTRIES may be zero in which case this
     * code is optimized out) */
    for (size_t i = 0; i < UNIVERSAL_ADDRESS_MAX_ENTRIES; ++i) {
        universal_address_print_entry(&universal_address_table[i]);
    }
}
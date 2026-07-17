static inline size_t base64_estimate_encode_size(size_t data_in_size)
{
    return (4 * ((data_in_size + 2) / 3));
}

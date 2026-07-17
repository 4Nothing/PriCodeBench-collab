int base64url_encode(const void *data_in, size_t data_in_size,
                     void *base64_out, size_t *base64_out_size)
{
    return base64_encode_base(data_in, data_in_size, base64_out, base64_out_size, true);
}
#
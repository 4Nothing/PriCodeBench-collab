void coding_xor_generate(void *data, size_t len, uint8_t *parity)
{
    _gen_parity(data, len, parity);
    _mix(data, len);
}
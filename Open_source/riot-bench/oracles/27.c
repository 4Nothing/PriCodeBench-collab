void bloom_init(bloom_t *bloom, size_t size, uint8_t *bitfield, hashfp_t *hashes, int hashes_numof)
{
    bloom->m = size;
    bloom->a = bitfield;
    bloom->hash = hashes;
    bloom->k = hashes_numof;
}

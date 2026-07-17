void sha3_384(void *digest, const void *data, size_t len)
{
    FIPS202_SHA3_384(data, len, digest);
}

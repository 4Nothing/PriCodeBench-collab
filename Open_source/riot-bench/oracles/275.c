void sha3_256(void *digest, const void *data, size_t len)
{
    FIPS202_SHA3_256(data, len, digest);
}

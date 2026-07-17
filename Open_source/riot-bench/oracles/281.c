void sha3_512(void *digest, const void *data, size_t len)
{
    FIPS202_SHA3_512(data, len, digest);
}

void bloom_del(bloom_t *bloom)
{
    if (bloom->a) {
        memset(bloom->a, 0, ROUND(bloom->m));
    }
    bloom->a = NULL;
    bloom->m = 0;
    bloom->hash = NULL;
    bloom->k = 0;
}

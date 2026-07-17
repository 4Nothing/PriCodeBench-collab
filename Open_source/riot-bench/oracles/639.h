static inline void *zptrd(zptr_t zptr)
{
    return (void *)(ZPTR_BASE + ((uint32_t)zptr << 2));
}
static inline zptr_t zptrc(void *pointer)
{
    assert(zptr_check(pointer));
    return (uint16_t)(((uint32_t)pointer - (uint32_t)ZPTR_BASE) >> 2);
}
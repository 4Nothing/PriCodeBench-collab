OS_SEC_ALW_INLINE INLINE U32 OsAtomicReadU32(volatile void *addr)
{
    return *(volatile U32 *)addr;
}
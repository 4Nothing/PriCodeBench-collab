OS_SEC_ALW_INLINE INLINE void OsAtomicSetU32(volatile void *addr, U32 val)
{
    *(volatile U32 *)addr = val;
}
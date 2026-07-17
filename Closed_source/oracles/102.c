OS_SEC_ALW_INLINE INLINE void OsHwiDescInitAll(void)
{
    U32 irqNum;
    for (irqNum = 0; irqNum < OS_HWI_MAX_NUM; irqNum++) {
        OsHwiDescFieldInit(irqNum);
    }
}
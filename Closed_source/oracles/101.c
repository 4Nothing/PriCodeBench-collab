OS_SEC_ALW_INLINE INLINE void OsHwiDescFieldInit(U32 irqNum)
{
    OsHwiFuncSet(irqNum, OsHwiDefaultHandler);
    OsHwiParaSet(irqNum, irqNum);
}
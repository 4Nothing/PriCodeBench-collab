OS_SEC_L4_TEXT U32 PRT_HwiSetAttr(HwiHandle hwiNum, HwiPrior hwiPrio, HwiMode mode)
{
    uintptr_t intSave;
    U32 irqNum;
    U32 ret;

    ret = OsHwiSetAttrParaCheck(hwiNum, hwiPrio, mode);
    if (ret != OS_OK) {
        return ret;
    }

    irqNum = OS_HWI2IRQ(hwiNum);

    OS_HWI_IRQ_LOCK(intSave);

#if defined(OS_OPTION_HWI_ATTRIBUTE)
    ret = OsHwiSetAttrConflictErrCheck(irqNum, hwiPrio, mode);
    if (ret != OS_OK) {
        OS_HWI_IRQ_UNLOCK(intSave);
        return ret;
    }
#endif

#if (defined(OS_OPTION_HWI_PRIORITY) && ((OS_GIC_VER == 3) || defined(OS_PLIC_VER)))
    if (OsHwiPriorityGet(hwiNum) != hwiPrio) {
        OsHwiPrioritySet(hwiNum, hwiPrio);
    }
#endif

    OsHwiAttrSet(irqNum, hwiPrio, mode);

    OS_HWI_IRQ_UNLOCK(intSave);
    return OS_OK;
}

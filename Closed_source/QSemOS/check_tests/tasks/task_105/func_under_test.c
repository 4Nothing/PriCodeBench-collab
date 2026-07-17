OS_SEC_L4_TEXT U32 PRT_HwiCreate(HwiHandle hwiNum, HwiProcFunc handler, HwiArg arg)
{
    uintptr_t intSave;
    U32 irqNum;
    U32 ret;

    if (OS_HWI_NUM_CHECK(hwiNum)) {
        return OS_ERRNO_HWI_NUM_INVALID;
    }

    if (handler == NULL) {
        return OS_ERRNO_HWI_PROC_FUNC_NULL;
    }

    irqNum = OS_HWI2IRQ(hwiNum);

    OS_HWI_IRQ_LOCK(intSave);

#if defined(OS_OPTION_HWI_ATTRIBUTE)
    if (OS_HWI_MODE_INV(irqNum)) {
        OS_HWI_IRQ_UNLOCK(intSave);
        return OS_ERRNO_HWI_MODE_UNSET;
    }
#endif

    ret = OsHwiConnectHandle(irqNum, handler, arg);
    if (ret != OS_OK) {
        OS_HWI_IRQ_UNLOCK(intSave);
        return ret;
    }

    OS_HWI_IRQ_UNLOCK(intSave);
    return OS_OK;
}

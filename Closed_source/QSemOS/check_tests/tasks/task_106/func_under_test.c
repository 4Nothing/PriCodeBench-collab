OS_SEC_L4_TEXT U32 PRT_HwiDelete(HwiHandle hwiNum)
{
    U32 ret;
    uintptr_t intSave;
    U32 irqNum;

    if (OS_HWI_NUM_CHECK(hwiNum)) {
        return OS_ERRNO_HWI_NUM_INVALID;
    }

    irqNum = OS_HWI2IRQ(hwiNum);

    /* 先disable中断 */
    (void)PRT_HwiDisable(hwiNum);

    OS_HWI_IRQ_LOCK(intSave);

    ret = OsHwiDeleteFormResume(irqNum);
    if (ret != OS_OK) {
        OS_HWI_IRQ_UNLOCK(intSave);
        return ret;
    }

    OS_HWI_IRQ_UNLOCK(intSave);
    return OS_OK;
}

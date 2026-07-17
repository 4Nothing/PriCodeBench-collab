OS_SEC_TEXT void OsHwiHookDispatcher(HwiHandle archHwi)
{
    /*
     * 将arch传入的参数转换成硬件中断号
     */
    HwiHandle hwiNum = OS_HWI_GET_HWINUM(archHwi);
    U32 irqNum = OS_HWI2IRQ(hwiNum);

    OS_MHOOK_ACTIVATE_PARA1(OS_HOOK_HWI_ENTRY, hwiNum);

    OsHwiHandleActive(irqNum);

    OS_MHOOK_ACTIVATE_PARA1(OS_HOOK_HWI_EXIT, hwiNum);
}

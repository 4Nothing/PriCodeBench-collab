OS_SEC_L4_TEXT void OsHwiDisableAll(void)
{
    U32 hwiNum;
    for (hwiNum = 0; hwiNum < OS_HWI_MAX_NUM; hwiNum++) {
        if (OS_HWI_NUM_CHECK(hwiNum)) {
            continue;
        }
        if (!OS_HWI_MODE_INV(hwiNum)) {
            /* 去使能中断 */
            PRT_HwiDisable(hwiNum);
#if defined(OS_ARCH_ARMV8)
            /* 清除pending位 */
            OsGicClearPendingBit(hwiNum);
            /* 清除active位 */
            OsGicClearActiveBit(hwiNum);
#endif
        }
    }
}

OS_SEC_L4_TEXT U32 OsHwiConfigInit(void)
{
#if defined(OS_OPTION_HWI_MAX_NUM_CONFIG)
    U32 ret = OsHwiResourceAlloc();
    if (ret != OS_OK) {
        return ret;
    }
#endif
    OS_HWI_SPINLOCK_INIT();

    OsHwiDescInitAll();

#if defined(OS_OPTION_HWI_COMBINE)
    /* 组合型中断结点初始化 */
    OsHwiCombineNodeInit();
#endif

    /* 硬件中断控制器初始化 */
    OsHwiGICInit();

    return OS_OK;
}

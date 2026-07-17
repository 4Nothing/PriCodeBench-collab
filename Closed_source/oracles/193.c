OS_SEC_L4_TEXT U32  OsTskInit(void)
{
    U32 ret;
    ret = OsTskSMPInit();
    if (ret != OS_OK) {
        return ret;
    }

    g_taskScanHook = OsTaskScan;

#if defined(OS_OPTION_TASK_INFO)
    g_excTaskInfoGet = (ExcTaskInfoFunc)OsTskInfoGet;
    g_taskNameAdd = OsTaskNameAdd;
    g_taskNameGet = OsTaskNameGet;
#endif

#if defined(OS_OPTION_POWEROFF)
    g_sysPowerOffHook = OsCpuPowerOff;
#endif

    /* 设置任务切换钩子标志位 */
    UNI_FLAG |= OS_FLG_TSK_SWHK;
    return OS_OK;
}

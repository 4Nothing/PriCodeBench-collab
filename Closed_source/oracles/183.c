OS_SEC_L4_TEXT U32 OsActivate(void)
{
    U32 ret;

    ret = OsIdleTskSMPCreate();

    if (ret != OS_OK) {
        return ret;
    }

    /* Indicate that background task is running. */
    UNI_FLAG |= OS_FLG_BGD_ACTIVE;

    if (OsGetHwThreadId() < (g_primaryCoreId + g_numOfCores - 1)) {

        OS_MHOOK_ACTIVATE_PARA0(OS_HOOK_FIRST_TIME_SWH);

        OsSmpWakeUpSecondaryCore();
    }

    /* Start Multitasking. */
    OsFirstTimeSwitch();

    return OS_ERRNO_TSK_ACTIVE_FAILED;
}

static U32 OsPerfTimedStop()
{
    U32 ret;
    if (PRT_GetCoreID() != g_primaryCoreId) {
        return OS_OK;
    }

    ret = PRT_TimerDelete(0, g_swtPmuId);
    if (ret != OS_OK) {
        printf("perf timer stop failed, ret = 0x%x\n", ret);
        return OS_ERROR;
    }

    return OS_OK;
}

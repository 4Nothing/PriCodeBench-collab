OS_SEC_L2_TEXT U64 OsCurCycleGet64(void)
{
    if (g_sysTimeHook != NULL) {
        return g_sysTimeHook();
    } else {
        return 0;
    }
}

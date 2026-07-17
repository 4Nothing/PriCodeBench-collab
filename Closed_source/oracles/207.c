OS_SEC_L2_TEXT bool PRT_IsLogInit()
{
    LOAD_FENCE();
    if (g_logMemBase != 0) {
        return true;
    }
    return false;
}

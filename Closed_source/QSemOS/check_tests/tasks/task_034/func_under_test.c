OS_SEC_L4_TEXT U32 OsSemRegister(const struct SemModInfo *modInfo)
{
    if (modInfo->maxNum == 0) {
        return OS_ERRNO_SEM_REG_ERROR;
    }

    g_maxSem = modInfo->maxNum;

    return OS_OK;
}

OS_SEC_L4_TEXT U32 OsTaskCreateParaCheck(const TskHandle *taskPid, struct TskInitParam *initParam)
{
    if ((taskPid == NULL) || (initParam == NULL)) {
        return OS_ERRNO_TSK_PTR_NULL;
    }

    if (initParam->taskEntry == NULL) {
        return OS_ERRNO_TSK_ENTRY_NULL;
    }

    if (initParam->stackSize == 0) {
        initParam->stackSize = g_tskModInfo.defaultSize;
    }

    if (((OS_TSK_STACK_SIZE_ALIGN - 1) & initParam->stackSize) != 0) {
        return OS_ERRNO_TSK_STKSZ_NOT_ALIGN;
    }

    if (((OS_TSK_STACK_SIZE_ALIGN - 1) & (uintptr_t)initParam->stackAddr) != 0) {
        return OS_ERRNO_TSK_STACKADDR_NOT_ALIGN;
    }

    if (initParam->stackSize < OS_TSK_MIN_STACK_SIZE) {
        return OS_ERRNO_TSK_STKSZ_TOO_SMALL;
    }
    /* 使用用户内存，则需要包含OS使用的资源，size最小值要包含OS的消耗 */
    if (initParam->stackAddr != 0) {
        if (OsCheckStackAddrOverflow(initParam->stackAddr, initParam->stackSize)) {
            return OS_ERRNO_TSK_STACKADDR_TOO_BIG;
        }
    }

    if (initParam->taskPrio > OS_TSK_PRIORITY_LOWEST) {
        return OS_ERRNO_TSK_PRIOR_ERROR;
    }

    /* 创建任务线程时做校验 */
    if (initParam->name == NULL || initParam->name[0] == '\0') {
        return OS_ERRNO_TSK_NAME_EMPTY;
    }

    return OS_OK;
}

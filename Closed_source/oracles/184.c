OS_SEC_L4_TEXT U32 OsTaskCreateOnly(TskHandle *taskPid, struct TskInitParam *initParam, bool isSmpIdle)
{
    U32 ret;
    U32 taskId;
    uintptr_t intSave;
    uintptr_t *topStack = NULL;
    void *stackPtr = NULL;
    struct TagTskCb *taskCb = NULL;
    uintptr_t curStackSize = 0;

    ret = OsTaskCreateParaCheck(taskPid, initParam);
    if (ret != OS_OK) {
        return ret;
    }

    intSave = OsIntLock();
    ret = OsTaskCreateChkAndGetTcb(&taskCb, isSmpIdle);
    if (ret != OS_OK) {
        OsIntRestore(intSave);
        return ret;
    }

    taskId = taskCb->taskPid;
    ret = OsTaskCreateRsrcInit(taskId, initParam, taskCb, &topStack, &curStackSize);
    if (ret != OS_OK) {
        OsTaskManageFreeCb(taskCb);
        OsIntRestore(intSave);
        return ret;
    }

    OsTskStackInit(curStackSize, (uintptr_t)topStack);

    stackPtr = OsTskContextInit(taskId, curStackSize, topStack, (uintptr_t)OsTskEntry);

    OsTskCreateTcbInit((uintptr_t)stackPtr, initParam, (uintptr_t)topStack, curStackSize, taskCb);

    OsTskCreateTcbStatusSet(taskCb, initParam);

    *taskPid = taskId;
    OsIntRestore(intSave);
    OS_MHOOK_ACTIVATE_PARA1(OS_HOOK_TSK_CREATE, taskId);
    return OS_OK;
}

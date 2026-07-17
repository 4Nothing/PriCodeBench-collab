OS_SEC_L4_TEXT U32 OsTaskNameAdd(U32 taskId, const char *name)
{
    struct TagTskCb *taskCb = GET_TCB_HANDLE(taskId);
    /* 不检测线程名同名 */
    if (strncpy_s(taskCb->name, sizeof(taskCb->name), name, OS_TSK_NAME_LEN - 1) != EOK) {
        OS_GOTO_SYS_ERROR1();
    }

    return OS_OK;
}

OS_SEC_L4_TEXT U32 PRT_TaskCreate(TskHandle *taskPid, struct TskInitParam *initParam)
{
    return OsTaskCreateOnly(taskPid, initParam, FALSE);
}

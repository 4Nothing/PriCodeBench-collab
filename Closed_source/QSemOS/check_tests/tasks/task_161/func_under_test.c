OS_SEC_L0_TEXT bool OsSemPendNotNeedSche(struct TagSemCb *semPended, struct TagTskCb *runTsk)
{
#if defined(OS_OPTION_SEM_RECUR_PV)
    if (GET_SEM_TYPE(semPended->semType) == SEM_TYPE_BIN && semPended->semOwner == runTsk->taskPid &&
        GET_MUTEX_TYPE(semPended->semType) == SEM_MUTEX_TYPE_RECUR) {
        semPended->recurCount++;
        return TRUE;
    }
#endif

    if (semPended->semCount > 0) {
        semPended->semCount--;
        semPended->semOwner = runTsk->taskPid;
#if defined(OS_OPTION_BIN_SEM)
        /* 如果是互斥信号量，把持有的互斥信号量挂接起来，此处需要受semPrio锁保护 */
        if (GET_SEM_TYPE(semPended->semType) == SEM_TYPE_BIN) {
            ListTailAdd(&semPended->semBList, &runTsk->semBList);
        }
#endif
        return TRUE;
    }
    return FALSE;
}

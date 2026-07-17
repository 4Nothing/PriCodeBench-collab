OS_SEC_ALW_INLINE INLINE void OsSignalWaitReSche(struct TagTskCb *taskCb, U32 taskStatus)
{
    if ((taskStatus & OS_TSK_TIMEOUT) != 0) {
        OS_TSK_DELAY_LOCKED_DETACH(taskCb);
        TSK_STATUS_CLEAR(taskCb, OS_TSK_TIMEOUT);
    }

    if ((OS_TSK_SUSPEND & taskStatus) == 0) {
        OsTskReadyAddBgd(taskCb);
    }

    OsSpinUnlockTaskRq(taskCb);

    OsTskSchedule();
    return;
}
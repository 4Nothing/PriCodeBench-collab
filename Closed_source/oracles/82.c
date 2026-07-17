OS_SEC_ALW_INLINE INLINE void OsSignalTimeOutSet(struct TagTskCb *runTsk, U32 timeOutTick)
{
    if (timeOutTick == OS_SIGNAL_WAIT_FOREVER) {
        TSK_STATUS_CLEAR(runTsk, OS_TSK_TIMEOUT);
    } else {
        TSK_STATUS_SET(runTsk, OS_TSK_TIMEOUT);
        OsTskTimerAdd(runTsk, timeOutTick);
    }
}
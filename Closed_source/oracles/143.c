OS_SEC_L0_TEXT void OsHwiEndCheckTimeSlice(U64 currTime)
{
    struct TagTskCb *currTsk = RUNNING_TASK;
    if (currTsk == NULL) {
        return;
    }
#if defined(OS_OPTION_RR_SCHED_IRQ_TIME_DISCOUNT)
    OsTimeSliceUpdate(currTsk, currTime, currTsk->irqUsedTime);
    currTsk->irqUsedTime = 0;
#else
    OsTimeSliceUpdate(currTsk, currTime, 0);
#endif
    if (currTsk->policy != OS_TSK_SCHED_RR) {
        return;
    }

    if (currTsk->timeSlice != 0) {
        return;
    }

    OsTskReadyDel(currTsk);
    /* 进队列后默认在队列尾部 */
    OsTskReadyAdd(currTsk);
    if ((g_highestTask != currTsk) && (g_uniTaskLock == 0)) {
        UNI_FLAG |= OS_FLG_TSK_REQ;
    }
    return;
}

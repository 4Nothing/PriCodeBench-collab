OS_SEC_L4_TEXT void OsTskCreateTcbInit(uintptr_t stackPtr, struct TskInitParam *initParam,
    uintptr_t topStackAddr, uintptr_t curStackSize, struct TagTskCb *taskCb)
{
    /* Initialize the task's stack */
    taskCb->stackPointer = (void *)stackPtr;
    taskCb->args[OS_TSK_PARA_0] = (uintptr_t)initParam->args[OS_TSK_PARA_0];
    taskCb->args[OS_TSK_PARA_1] = (uintptr_t)initParam->args[OS_TSK_PARA_1];
    taskCb->args[OS_TSK_PARA_2] = (uintptr_t)initParam->args[OS_TSK_PARA_2];
    taskCb->args[OS_TSK_PARA_3] = (uintptr_t)initParam->args[OS_TSK_PARA_3];
    taskCb->topOfStack = topStackAddr;
    taskCb->stackSize = curStackSize;
    taskCb->taskPend = NULL;
    taskCb->priority = initParam->taskPrio;
    taskCb->origPriority = initParam->taskPrio;
    taskCb->taskEntry = initParam->taskEntry;
#if defined(OS_OPTION_EVENT)
    taskCb->event = 0;
    taskCb->eventMask = 0;
#endif
    taskCb->lastErr = 0;
#if defined(OS_OPTION_RR_SCHED)
    if (initParam->policy == OS_TSK_SCHED_RR) {
        taskCb->policy = OS_TSK_SCHED_RR;
    } else {
        taskCb->policy = OS_TSK_SCHED_FIFO;
    }
    taskCb->startTime = 0;
    taskCb->timeSlice = g_timeSliceCycle;
#if defined(OS_OPTION_RR_SCHED_IRQ_TIME_DISCOUNT)
    taskCb->irqUsedTime = 0;
#endif
#endif

    OS_LIST_INIT(&taskCb->semBList);
    OS_LIST_INIT(&taskCb->pendList);
    OS_LIST_INIT(&taskCb->timerList);

#if defined(OS_OPTION_POSIX)
    taskCb->tsdUsed = 0;
    taskCb->state = PTHREAD_CREATE_JOINABLE;
    taskCb->cancelState = PTHREAD_CANCEL_ENABLE;
    taskCb->cancelType = PTHREAD_CANCEL_DEFERRED;
    taskCb->cancelPending = 0;
    taskCb->cancelBuf = NULL;
    taskCb->retval = NULL;
    taskCb->joinCount = 0;
    taskCb->joinableSem = 0;
#if defined(OS_OPTION_POSIX_SIGNAL)
    taskCb->sigMask = 0;
    taskCb->sigWaitMask = 0;
    taskCb->sigPending = 0;
    taskCb->itimer = 0;
    INIT_LIST_OBJECT(&taskCb->sigInfoList);
    OsInitSigVectors(taskCb);
#endif
#if defined(OS_OPTION_LOCALE)
    taskCb->locale = (locale_t)libc_global_locale;
#endif
#endif
    OsTskSMPTcbInit(initParam, taskCb);
    return;
}

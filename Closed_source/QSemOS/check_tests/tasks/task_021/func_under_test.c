OS_SEC_ALW_INLINE INLINE void OsSwTmrCreateTimerCbInit(struct TimerCreatePara *createPara,
                                                       struct TagSwTmrCtrl *swtmr, U32 interval)
{
    /* 设置新定时器的参数 */
    swtmr->handler = createPara->callBackFunc;
    swtmr->mode = (U8)createPara->mode;
    swtmr->interval = interval;
    swtmr->next = NULL;
    swtmr->prev = NULL;
    swtmr->idxRollNum = swtmr->interval;
    swtmr->overrun = 0;
    swtmr->arg1 = createPara->arg1;
    swtmr->arg2 = createPara->arg2;
    swtmr->arg3 = createPara->arg3;
    swtmr->arg4 = createPara->arg4;
    swtmr->state = (U8)OS_TIMER_CREATED;
#if defined(OS_OPTION_SMP)
    swtmr->coreID = OsGetCoreID();
#endif
    return;
}
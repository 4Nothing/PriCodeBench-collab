OS_SEC_L4_TEXT U32 OsSwTmrCreateTimer(struct TimerCreatePara *createPara, TimerHandle *tmrHandle)
{
    struct TagSwTmrCtrl *swtmr = NULL;
    U32 interval;
    uintptr_t intSave;
    U32 ret;

    ret = OsSwTmrCreateTimerParaChk(createPara);
    if (ret != OS_OK) {
        return ret;
    }

    interval = (U32)DIV64(((U64)g_tickModInfo.tickPerSecond * (createPara->interval)), OS_SYS_MS_PER_SECOND);

    intSave = OsIntLock();

#if defined(OS_OPTION_SMP)
    SWTMR_CREATE_DEL_LOCK();
    if (g_tmrFreeList.freeList == NULL) {
        SWTMR_CREATE_DEL_UNLOCK();
        OsIntRestore(intSave);
        return OS_ERRNO_SWTMR_MAXSIZE;
    }

    /*
     * 从空闲链表中取出一个控制块
     * 保证空闲链表的完整性，首节点没有前驱，尾节点没有后继节点
     */
    swtmr = g_tmrFreeList.freeList;
    g_tmrFreeList.freeList = swtmr->next;
#else

    if (g_tmrFreeList == NULL) {
        OsIntRestore(intSave);
        return OS_ERRNO_SWTMR_MAXSIZE;
    }

    /*
     * 从空闲链表中取出一个控制块
     * 保证空闲链表的完整性，首节点没有前驱，尾节点没有后继节点
     */
    swtmr = g_tmrFreeList;
    g_tmrFreeList = swtmr->next;
#endif

    OsSwTmrCreateTimerCbInit(createPara, swtmr, interval);

    SWTMR_CREATE_DEL_UNLOCK();
    OsIntRestore(intSave);
    /* 把TimerID写入返回参数 */
    *tmrHandle = OS_SWTMR_INDEX_2_ID(swtmr->swtmrIndex);

    return OS_OK;
}

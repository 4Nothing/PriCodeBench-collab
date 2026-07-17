OS_SEC_L4_TEXT U32 OsSwTmrResInit(void)
{
    U32 ret;
    U32 size;
    U32 idx;
    // struct TagListObject *listObject = NULL;
    struct TagSwTmrCtrl *swtmr = NULL;

    /*
     * 为定时器控制块分配内存，内存首地址保存到g_swtmrCbArray
     * 控制块所需内存超过0xFFFFFFFF
     */
    if (g_swTmrMaxNum > (OS_MAX_U32 / sizeof(struct TagSwTmrCtrl))) {
        return OS_ERRNO_SWTMR_NO_MEMORY;
    }

    /* 为SortLink分配内存，初始化g_tmrSortLink */
    struct TagListObject *listObject = NULL;
    
    size = sizeof(struct TagListObject) * OS_SWTMR_SORTLINK_LEN;
    listObject =
        (struct TagListObject *)OsMemAllocAlign((U32)OS_MID_SWTMR, OS_MEM_DEFAULT_FSC_PT, size, MEM_ADDR_ALIGN_016);
    if (listObject == NULL) {
        return OS_ERRNO_SWTMR_NO_MEMORY;
    }

    if (memset_s(listObject, size, 0, size) != EOK) {
        OS_GOTO_SYS_ERROR1();
    }
    g_tmrSortLink.sortLink = listObject;
    g_tmrSortLink.cursor = 0;

    for (idx = 0; idx < OS_SWTMR_SORTLINK_LEN; idx++, listObject++) {
        listObject->next = listObject;
        listObject->prev = listObject;
    }

    size = sizeof(struct TagSwTmrCtrl) * g_swTmrMaxNum;
    swtmr = (struct TagSwTmrCtrl *)OsMemAllocAlign((U32)OS_MID_SWTMR, OS_MEM_DEFAULT_FSC_PT, size, MEM_ADDR_ALIGN_016);
    if (swtmr == NULL) {
        ret = PRT_MemFree((U32)OS_MID_SWTMR, (void *)g_tmrSortLink.sortLink);
        if (ret != OS_OK) {
            OS_REPORT_ERROR(ret);
        }

        return OS_ERRNO_SWTMR_NO_MEMORY;
    }

    if (memset_s(swtmr, size, 0, size) != EOK) {
        OS_GOTO_SYS_ERROR1();
    }
    g_swtmrCbArray = swtmr;

    /* 把所有定时器控制块组织成一条单向链表，存放到空闲链中，空闲了首地址g_tmrFreeList */
    g_tmrFreeList = swtmr;
    OsSwTmrCtrlInit(swtmr);

#if defined(OS_OPTION_TICKLESS)
    g_getSwtmrNearestTick = OsSwtmrNearestTickGet;
    g_checkSwtmrTickProcess = OsSwtmrTargetCheck;
#endif

    return OS_OK;
}

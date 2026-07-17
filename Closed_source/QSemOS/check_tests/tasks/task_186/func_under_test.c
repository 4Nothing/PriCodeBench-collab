OS_SEC_ALW_INLINE INLINE void OsTaskManageFreeCb(struct TagTskCb *taskCB)
{
    TSK_CREATE_DEL_LOCK();

    ListAdd(&taskCB->pendList, &g_tskCbFreeList);

    TSK_CREATE_DEL_UNLOCK();
}
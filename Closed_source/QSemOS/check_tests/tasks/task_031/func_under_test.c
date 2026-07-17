OS_SEC_ALW_INLINE INLINE U32 OsSemCreateCb(U32 count, U32 semType, enum SemMode semMode,
                                           SemHandle *semHandle, U32 cookie)
{
    struct TagSemCb *semCreated = NULL;
    struct TagListObject *unusedSem = NULL;
    (void)cookie;

    if (ListEmpty(&g_unusedSemList)) {
        return OS_ERRNO_SEM_ALL_BUSY;
    }

    /* 在空闲链表中取走一个控制节点 */
    unusedSem = OS_LIST_FIRST(&(g_unusedSemList));
    ListDelete(unusedSem);

    /* 获取到空闲节点对应的信号量控制块，并开始填充控制块 */
    semCreated = (GET_SEM_LIST(unusedSem));

    OsSemCreateCbInit(count, semType, semMode, semCreated);

    *semHandle = (SemHandle)semCreated->semId;

    return OS_OK;
}
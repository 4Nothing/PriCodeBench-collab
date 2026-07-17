OS_SEC_ALW_INLINE INLINE void OsSemCreateCbInit(U32 count, U32 semType, enum SemMode semMode,
                                           struct TagSemCb *semCreated)
{
    semCreated->semCount = count;
    semCreated->semStat = OS_SEM_USED;
    semCreated->semMode = semMode;
    semCreated->semType = semType;
    semCreated->semOwner = OS_INVALID_OWNER_ID;
    if (GET_SEM_TYPE(semType) == SEM_TYPE_BIN) {
        OS_LIST_INIT(&semCreated->semBList);
#if defined(OS_OPTION_SEM_RECUR_PV)
        semCreated->recurCount = 0;
#endif
    }

    OS_LIST_INIT(&semCreated->semList);
}
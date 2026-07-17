U32 OsRwLockGetMode(struct TagListObject *readList, struct TagListObject *writeList)
{
    bool isReadEmpty = ListEmpty(readList);
    bool isWriteEmpty = ListEmpty(writeList);
    if (isReadEmpty && isWriteEmpty) {
        return RWLOCK_NONE_MODE;
    }
    if (!isReadEmpty && isWriteEmpty) {
        return RWLOCK_READ_MODE;
    }
    if (isReadEmpty && !isWriteEmpty) {
        return RWLOCK_WRITE_MODE;
    }

    struct TagTskCb *pendedReadTask = GET_TCB_PEND(OS_LIST_FIRST(readList));
    struct TagTskCb *pendedWriteTask = GET_TCB_PEND(OS_LIST_FIRST(writeList));
    if (pendedWriteTask->priority <= pendedReadTask->priority) {
        return RWLOCK_WRITEFIRST_MODE;
    }
    return RWLOCK_READFIRST_MODE;
}

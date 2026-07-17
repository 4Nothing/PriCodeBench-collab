struct TagListObject *OsRwLockPendFindPosSub(const struct TagTskCb *runTask, const struct TagListObject *lockList)
{
    struct TagTskCb *pendedTask;
    struct TagListObject *node = NULL;

    LIST_FOR_EACH_SAFE(pendedTask, lockList, struct TagTskCb, pendList) {
        if (pendedTask->priority < runTask->priority) {
            continue;
        } else if (pendedTask->priority > runTask->priority) {
            node = &pendedTask->pendList;
            break;
        } else {
            node = pendedTask->pendList.next;
            break;
        }
    }

    return node;
}

OS_SEC_L0_TEXT void OsTskReadyAdd(struct TagTskCb *task)
{
    OsTskReadyAddOnly(task);

    OsReschedTask(task);

    return;
}

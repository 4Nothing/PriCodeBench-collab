OS_SEC_L0_TEXT void OsTskReadyAddNoWakeUpIpc(struct TagTskCb *task)
{
    OsTskReadyAddOnly(task);
    OsReschedTaskNoWakeIpc(task);
    return;
}

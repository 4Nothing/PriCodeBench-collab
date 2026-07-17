OS_SEC_L2_TEXT uintptr_t OsTaskSpInfoGet(struct TagTskCb *taskCb)
{
    uintptr_t sp;

    if (OsTaskSpPcInfoReady(taskCb)) {
        sp = (uintptr_t)taskCb->stackPointer;
    } else {
        sp = OsGetSp();
    }

    return sp;
}

OS_SEC_L2_TEXT void OsTaskSpPcGet(struct TagTskCb *taskCb, struct TskInfo *taskInfo)
{
    if (OsTaskSpPcInfoReady(taskCb)) {
        taskInfo->sp = (uintptr_t)taskCb->stackPointer;
        taskInfo->pc = OsTskGetInstrAddr((uintptr_t)taskCb->stackPointer);
    }
    /* 非中断时, 任务若处于运行态, 其控制块的stackPointer值不准确,不能据此获取SP,PC */
    return;
}

OS_SEC_L4_TEXT U32 PRT_QueueCreate(U16 nodeNum, U16 maxNodeSize, U32 *queueId)
{
    uintptr_t intSave;
    U32 ret;
    U32 qId = 0;

    ret = OsQueueCreatParaCheck(nodeNum, maxNodeSize, queueId);
    if (ret != OS_OK) {
        return ret;
    }

    QUEUE_INIT_IRQ_LOCK(intSave);
    ret = OsQueueCreate(nodeNum, maxNodeSize, &qId);
    if (ret != OS_OK) {
        OsIntRestore(intSave);
        return ret;
    }

    *queueId = qId;
    QUEUE_INIT_IRQ_UNLOCK(intSave);
    return OS_OK;
}

OS_SEC_L4_TEXT U32 PRT_QueueRead(U32 queueId, void *bufferAddr, U32 *len, U32 timeOut)
{
    uintptr_t intSave;
    U32 ret;
    U32 bufLen;
    U32 innerId = OS_QUEUE_INNER_ID(queueId);
    struct TagQueCb *queueCb = NULL;
    struct QueNode *queueNode = NULL;

    if (innerId >= g_maxQueue) {
        return OS_ERRNO_QUEUE_INVALID;
    }

    if ((bufferAddr == NULL) || (len == NULL)) {
        return OS_ERRNO_QUEUE_PTR_NULL;
    }

    if (*len == 0) {
        return OS_ERRNO_QUEUE_SIZE_ZERO;
    }

    bufLen = *len;
    /* 获取指定队列控制块 */
    queueCb = (struct TagQueCb *)GET_QUEUE_HANDLE(innerId);

    QUEUE_CB_IRQ_LOCK(queueCb, intSave);
    if (queueCb->queueState == OS_QUEUE_UNUSED) {
        ret = OS_ERRNO_QUEUE_NOT_CREATE;
        goto QUEUE_END;
    }

    /* 读队列PEND */
    ret = OsInnerPend(&queueCb->readableCnt, &queueCb->readList, timeOut, queueCb);
    if (ret != OS_OK) {
        goto QUEUE_END;
    }

    /* 获取该队列中第一个有数据结点首地址，并将数据存入buffer中 */
    queueNode = (struct QueNode *)(uintptr_t)&queueCb->queue[(queueCb->queueHead) * (queueCb->nodeSize)];

    /* 如果buf的长度小于数据长度，则仅返回buf大小的数据，如果大于，则返回全部数据 */
    if (*len > queueNode->size) {
        *len = queueNode->size;
    }

    if (memcpy_s(bufferAddr, bufLen, (void *)queueNode->buf, *len) != EOK) {
        OS_GOTO_SYS_ERROR1();
    }

    queueNode->srcPid = OS_QUEUE_PID_INVALID;

    /* 队列头指针加1 */
    queueCb->queueHead++;
    /* 如果队列头指针已经到队列尾，那么头指针指向队列头 */
    if (queueCb->queueHead == queueCb->nodeNum) {
        queueCb->queueHead = 0;
    }

    if (OsQueuePendNeedProc(&queueCb->writeList)) {
        QUEUE_CB_UNLOCK(queueCb);
        OsTskSchedule();
        OsIntRestore(intSave);
        return OS_OK;
    }

    queueCb->writableCnt++;

QUEUE_END:
    QUEUE_CB_IRQ_UNLOCK(queueCb, intSave);
    return ret;
}

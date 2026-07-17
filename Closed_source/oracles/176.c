OS_SEC_L4_TEXT U32 PRT_QueueWrite(U32 queueId, void *bufferAddr, U32 bufferSize, U32 timeOut, U32 prio)
{
    U32 ret;
    uintptr_t intSave;
    U32 innerId = OS_QUEUE_INNER_ID(queueId);
    struct TagQueCb *queueCb = NULL;

    ret = OsQueueWriteParaCheck(innerId, (uintptr_t)bufferAddr, bufferSize, prio);
    if (ret != OS_OK) {
        return ret;
    }

    /* 获取指定队列控制块 */
    queueCb = (struct TagQueCb *)GET_QUEUE_HANDLE(innerId);
    QUEUE_CB_IRQ_LOCK(queueCb, intSave);
    if (queueCb->queueState == OS_QUEUE_UNUSED) {
        ret = OS_ERRNO_QUEUE_NOT_CREATE;
        goto QUEUE_END;
    }

    if (bufferSize > (queueCb->nodeSize - OS_QUEUE_NODE_HEAD_LEN)) {
        ret = OS_ERRNO_QUEUE_SIZE_TOO_BIG;
        goto QUEUE_END;
    }

    /* 读队列PEND */
    ret = OsInnerPend(&queueCb->writableCnt, &queueCb->writeList, timeOut, queueCb);
    if (ret != OS_OK) {
        goto QUEUE_END;
    }

    /* 选取消息节点，初始化消息节点拷贝数据 */
    OsQueueCpData2Node(prio, (uintptr_t)bufferAddr, bufferSize, queueCb);

    if (OsQueuePendNeedProc(&queueCb->readList)) {
        QUEUE_CB_UNLOCK(queueCb);
        OsTskSchedule();
        OsIntRestore(intSave);
        return OS_OK;
    }

    queueCb->readableCnt++;

QUEUE_END:
    QUEUE_CB_IRQ_UNLOCK(queueCb, intSave);
    return ret;
}

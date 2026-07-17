static inline void OsQueueCpData2Node(U32 prio, uintptr_t bufferAddr, U32 bufferSize,
                                       struct TagQueCb *queueCb)
{
    U16 peak;
    struct QueNode *queueNode = NULL;
    if (prio == (U32)OS_QUEUE_NORMAL) {
        /* 普通消息加到队列尾部 */
        queueNode = (struct QueNode *)(uintptr_t)&queueCb->queue[((queueCb->queueTail) * (queueCb->nodeSize))];

        queueCb->queueTail++;
        if (queueCb->queueTail == queueCb->nodeNum) {
            queueCb->queueTail = 0;
        }
    } else {
        /* 紧急消息加到队列头上 */
        if (queueCb->queueHead == 0) {
            queueCb->queueHead = queueCb->nodeNum;
        }
        queueCb->queueHead--;

        queueNode = (struct QueNode *)(uintptr_t)&queueCb->queue[((queueCb->queueHead) * (queueCb->nodeSize))];
    }

    if (memcpy_s((void *)queueNode->buf, (queueCb->nodeSize - OS_QUEUE_NODE_HEAD_LEN),
                 (void *)bufferAddr, bufferSize) != EOK) {
        OS_GOTO_SYS_ERROR1();
    }

    queueNode->size = (U16)bufferSize;
    queueNode->srcPid = (U16)OsGetSrcPid();

    peak = queueCb->queueTail > queueCb->queueHead ? queueCb->queueTail - queueCb->queueHead
           : (queueCb->nodeNum - queueCb->queueHead) + queueCb->queueTail;

    if (peak > queueCb->nodePeak) {
        queueCb->nodePeak = peak;
    }
}
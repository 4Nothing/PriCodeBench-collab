OS_SEC_L4_TEXT U32 OsQueueCreate(U16 nodeNum, U16 maxNodeSize, U32 *queueId)
{
    U32 index;
    U32 qId = 0;
    struct QueNode *queueNode = NULL;
    U16 nodeSize = maxNodeSize;
    struct TagQueCb *queueCb = NULL;

    /* 获取一个空闲的队列资源 */
    queueCb = g_allQueue;
    for (index = 0; index < g_maxQueue; index++, queueCb++) {
        if (queueCb->queueState == OS_QUEUE_UNUSED) {
            qId = OS_QUEUE_ID(index);
            break;
        }
    }

    if (index == g_maxQueue) {
        return OS_ERRNO_QUEUE_CB_UNAVAILABLE;
    }

    nodeSize = (U16)OsQueueGetNodeSize(nodeSize);
    queueCb->queue = (U8 *)OsMemAlloc(OS_MID_QUEUE, OS_MEM_DEFAULT_FSC_PT, (U32)nodeNum * (U32)nodeSize);
    if (queueCb->queue == NULL) {
        return OS_ERRNO_QUEUE_CREATE_NO_MEMORY;
    }

    for (index = 0; index < nodeNum; index++) {
        queueNode = (struct QueNode *)(uintptr_t)&queueCb->queue[index * nodeSize];
        queueNode->srcPid = OS_QUEUE_PID_INVALID;
    }

    queueCb->nodeNum = nodeNum;
    queueCb->nodeSize = nodeSize;
    queueCb->queueState = OS_QUEUE_USED;
    INIT_LIST_OBJECT(&queueCb->writeList);
    INIT_LIST_OBJECT(&queueCb->readList);
    queueCb->writableCnt = nodeNum;
    queueCb->queueHead = 0;
    queueCb->queueTail = 0;
    queueCb->nodePeak = 0;
    queueCb->readableCnt = 0;

    *queueId = qId;

    return OS_OK;
}

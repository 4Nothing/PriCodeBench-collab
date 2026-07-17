OS_SEC_L4_TEXT U32 OsQueueCreatParaCheck(U16 nodeNum, U16 nodeSize, const U32 *queueId)
{
    if (queueId == NULL) {
        return OS_ERRNO_QUEUE_CREAT_PTR_NULL;
    }

    if ((nodeNum == 0) || (nodeSize == 0)) {
        return OS_ERRNO_QUEUE_PARA_ZERO;
    }

    if (OsQueueGetNodeSize(nodeSize) > OS_MAX_U16) {
        return OS_ERRNO_QUEUE_NSIZE_INVALID;
    }
    return OS_OK;
}

OS_SEC_L4_TEXT U32 OsQueueRegister(U16 maxQueue)
{
    if (maxQueue == 0) {
        return OS_ERRNO_QUEUE_MAXNUM_ZERO;
    }

    g_maxQueue = maxQueue;
    return OS_OK;
}

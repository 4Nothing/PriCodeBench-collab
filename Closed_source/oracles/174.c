OS_SEC_L4_TEXT U32 OsQueueWriteParaCheck(U32 innerId, uintptr_t bufferAddr, U32 bufferSize, U32 prio)
{
    if (innerId >= g_maxQueue) {
        return OS_ERRNO_QUEUE_INVALID;
    }

    if (bufferAddr == 0) {
        return OS_ERRNO_QUEUE_PTR_NULL;
    }

    if (bufferSize == 0) {
        return OS_ERRNO_QUEUE_SIZE_ZERO;
    }

    if (prio > (U32)OS_QUEUE_URGENT) {
        return OS_ERRNO_QUEUE_PRIO_INVALID;
    }

    return OS_OK;
}

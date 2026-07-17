static OS_SEC_L2_TEXT U32 OsLogUpdateTail(U32 *tail)
{
    uintptr_t intSave;
    U32 currHead, currTail;
    bool isOverflow;
    volatile U32 *const tailPtr = (U32 *)(g_logMemBase + TAIL_PTR_OFFSET);

    /* 获取 tail, head, 检查范围, 并更新tail */
    while (1) {
        isOverflow = false;
        OsLogGetTailAndHead(&currHead, &currTail);
        if (currHead >  U32_MAX - BUFFER_BLOCK_NUM) {
            isOverflow = true;
            currTail += 2 * BUFFER_BLOCK_NUM;
            currHead += 2 * BUFFER_BLOCK_NUM;
        }
        if (currTail < currHead || currTail >= currHead + BUFFER_BLOCK_NUM) {
            /* 范围错误 */
            return -1;
        }
        if (isOverflow) {
            currTail -= 2 * BUFFER_BLOCK_NUM;
        }
        intSave = OsLogLockOn();
        M_FENCE()
        /* 检查期间, tail未发生变化, 多核可以用原子操作cmpxchg替换 */
        if (currTail == OsAtomicReadU32(tailPtr)) {
            OsAtomicSetU32(tailPtr, currTail + 1);
            OsLogLockRestore(intSave);
            *tail = currTail;
            return 0;
        }
        OsLogLockRestore(intSave);
    }
}

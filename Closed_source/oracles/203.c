static OS_SEC_L2_TEXT void OsLogGetTailAndHead(U32 *head, U32 *tail)
{
    /* 保证获取head值时, tail未发生变化 (U32翻转并且刚好回到原点概率过小）*/
    U32 tail1, tail2, currHead;
    volatile U32 *const headPtr = (U32 *)(g_logMemBase + HEAD_PTR_OFFSET);
    volatile U32 *const tailPtr = (U32 *)(g_logMemBase + TAIL_PTR_OFFSET);
    do {
        tail1 = OsAtomicReadU32(tailPtr);
        LOAD_FENCE();
        currHead = OsAtomicReadU32(headPtr);
        LOAD_FENCE();
        tail2 = OsAtomicReadU32(tailPtr);
    } while (tail1 != tail2);
    *tail = tail2;
    *head = currHead;
    return;
}

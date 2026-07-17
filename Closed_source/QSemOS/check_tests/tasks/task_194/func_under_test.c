OS_SEC_L4_TEXT void *OsTskMemAlloc(U32 size)
{
    void *stackAddr = NULL;
    stackAddr = OsMemAllocAlign((U32)OS_MID_TSK, (U8)OS_MEM_DEFAULT_FSC_PT, size,
                                /* 内存已按16字节大小对齐 */
                                OS_TSK_STACK_SIZE_ALLOC_ALIGN);
    return stackAddr;
}

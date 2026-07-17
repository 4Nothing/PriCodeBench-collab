OS_SEC_L4_TEXT U32 OsQueueConfigInit(void)
{
    void *addr = NULL;

    addr = OsMemAlloc(OS_MID_QUEUE, OS_MEM_DEFAULT_FSC_PT, g_maxQueue * sizeof(struct TagQueCb));
    if (addr == NULL) {
        return OS_ERRNO_QUEUE_NO_MEMORY;
    }

    if (memset_s(addr, g_maxQueue * sizeof(struct TagQueCb), 0, g_maxQueue * sizeof(struct TagQueCb)) != EOK) {
        OS_GOTO_SYS_ERROR1();
    }

    g_allQueue = (struct TagQueCb *)addr;

    return OS_OK;
}

static OS_SEC_L2_TEXT U32 OsLog(enum OsLogLevel level, enum OsLogFacility facility, const char *str, size_t strLen)
{
    uintptr_t intSave;
    U32 currTail, logIndex, sequenceNum, ret;
    U8 validFlag;
    U8 *targetMem;
    struct logHeader header;
    TskHandle taskPid = -1;
    struct timespec ts = {0, 0};
    volatile U8 *const validPtr = (U8 *)(g_logMemBase + VALID_FLAGS_OFFSET);

    intSave = OsLogLockOn();
    /* 多核可以用原子自增替换 */
    sequenceNum = g_sequenceNum;
    g_sequenceNum++;
    OsLogLockRestore(intSave);

    if (OsLogUpdateTail(&currTail)) {
        return -1;
    }

    /* 计算当前的有效标记位, 奇数圈1, 偶数圈0 */
    logIndex = currTail % (2 * BUFFER_BLOCK_NUM);
    if (logIndex < BUFFER_BLOCK_NUM) {
        validFlag = 1;
    } else {
        validFlag = 0;
    }
    logIndex = logIndex % (BUFFER_BLOCK_NUM);

    clock_gettime(CLOCK_REALTIME, &ts);
    header.sec = ts.tv_sec;
    header.nanoSec = ts.tv_nsec;
    header.sequenceNum = sequenceNum;
    PRT_TaskSelf(&taskPid);
    header.taskPid = taskPid;
    header.len = strLen;
    header.facility = facility;
    header.level = level;

    targetMem = (U8 *)g_logMemBase + (logIndex * BUFFER_BLOCK_SIZE);
    (void)memcpy_s(targetMem, sizeof(struct logHeader), &header, sizeof(struct logHeader));
    ret = memcpy_s(targetMem + sizeof(struct logHeader), LOG_MAX_SIZE, str, strLen);

    STORE_FENCE();

    /* 设置有效标记位 */
    *(volatile U8 *)(validPtr + logIndex) = validFlag;
    if (ret != EOK) {
        return -1;
    }
    /* 保证有效标记位及时更新，不然最后一个日志可能获取较慢 */
    STORE_FENCE();
    return 0;
}
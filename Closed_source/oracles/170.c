extern inline U32 OsGetSrcPid(void);

OS_SEC_ALW_INLINE INLINE U32 OsGetSrcPid(void)
{
    U32 srcPid;

    if (OS_HWI_ACTIVE) {
        /* 硬中断创建消息不具体区别中断号 */
#if defined(OS_OPTION_SMP)
        srcPid = COMPOSE_PID(OsGetHwThreadId(), OS_HWI_HANDLE);
#else
        srcPid = COMPOSE_PID(0x0U, OS_HWI_HANDLE);
#endif
    } else {
        srcPid = RUNNING_TASK->taskPid;
    }

    return srcPid;
}
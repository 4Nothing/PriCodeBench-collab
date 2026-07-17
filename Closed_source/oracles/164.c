static INLINE bool OsSemPostIsInvalid(struct TagSemCb *semPosted)
{
    if (GET_SEM_TYPE(semPosted->semType) == SEM_TYPE_BIN) {
#if defined(OS_OPTION_SEM_RECUR_PV)
        if (GET_MUTEX_TYPE(semPosted->semType) == SEM_MUTEX_TYPE_RECUR && semPosted->recurCount > 0) {
            semPosted->recurCount--;
            return TRUE;
        }
#endif
        /* 释放互斥二进制信号量且信号量已处于空闲状态 */
        if ((semPosted)->semCount == OS_SEM_FULL) {
            return TRUE;
        }
    }
    return FALSE;
}
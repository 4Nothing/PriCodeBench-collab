OS_SEC_L0_TEXT void OsContextSwitch(struct TagTskCb *prev, struct TagTskCb *next)
{
    /* 有任务切换钩子&最高优先级任务等待调度 */
    if (prev != next) {
        OsTskSwitchHookCaller(prev->taskPid, next->taskPid);
    }
    /* 正式切换,prev已经在putprev经过处理，这里跳过 */
    OsTskContextLoad((uintptr_t)next);
}

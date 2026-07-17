static void OsPerfSwtimer(TimerHandle tmrHandle, U32 arg1, U32 arg2, U32 arg3, U32 arg4)
{
    /* send to all cpu to collect data */
    SMP_CALL_PERF_FUNC(OsPerfTimedHandle);
    return;
}

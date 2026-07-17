OS_SEC_L2_TEXT void PRT_ClkDelayMs(U32 delay)
{
    U64 cycles;

    /* 将毫秒转化为Cycle计数，例如1m为1M个cycle */
    cycles = OS_MS2CYCLE(delay, g_systemClock);

    OsTimerDelayCount(cycles);
}

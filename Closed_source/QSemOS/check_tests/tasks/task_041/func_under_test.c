OS_SEC_L4_TEXT void PRT_ClkDelayUs(U32 delay)
{
    U64 cycles;

    cycles = OS_US2CYCLE(delay, g_systemClock);

    OsTimerDelayCount(cycles);
}

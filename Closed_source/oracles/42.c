OS_SEC_L2_TEXT U64 PRT_TickGetCount(void)
{
    return (U64)(g_uniTicks + g_ticksOffset);
}

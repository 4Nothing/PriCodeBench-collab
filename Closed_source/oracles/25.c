OS_SEC_L4_TEXT U32 OsSwTmrInit(U32 maxTimerNum)
{
    struct TmrGrpUserCfg config;

    config.tmrGrpSrcType = OS_TIMER_GRP_SRC_TICK;
    config.maxTimerNum = maxTimerNum;

    return OsSwTimerGroupCreate(&config, &g_tickSwTmrGroupId);
}

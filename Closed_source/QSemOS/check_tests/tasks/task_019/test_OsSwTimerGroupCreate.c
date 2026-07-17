#include "prt_swtmr_internal.h"
#include "test_util.h"
#include "func_under_test.c"

// /* OsSwTimerGroupCreate now from source */


int main(void)
{
    TU_reset();

    // Case 1: tick not init
    struct TmrGrpUserCfg cfg = {OS_TIMER_GRP_SRC_TICK, 10};
    TimerGroupId gid = 0;
    g_tickModInfo.tickPerSecond = 0;
    g_timerApi[TIMER_TYPE_SWTMR].createTimer = NULL;
    U32 r1 = OsSwTimerGroupCreate(&cfg, &gid);
    TU_check_u32("tick not init", OS_ERRNO_TICK_NOT_INIT, r1);

    // Case 2: maxTimerNum == 0
    g_tickModInfo.tickPerSecond = 1000;
    cfg.maxTimerNum = 0;
    gid = 0;
    U32 r2 = OsSwTimerGroupCreate(&cfg, &gid);
    TU_check_u32("max num zero", OS_ERRNO_TIMER_NUM_ZERO, r2);

    // Case 3: maxTimerNum too large ( > U16 )
    cfg.maxTimerNum = (U32)OS_MAX_U16 + 1;
    U32 r3 = OsSwTimerGroupCreate(&cfg, &gid);
    TU_check_u32("max num too large", OS_ERRNO_TIMER_NUM_TOO_LARGE, r3);

    // Case 4: already created
    cfg.maxTimerNum = 8;
    g_timerApi[TIMER_TYPE_SWTMR].createTimer = (void *)0x1; // non-NULL
    U32 r4 = OsSwTimerGroupCreate(&cfg, &gid);
    TU_check_u32("already created", OS_ERRNO_TIMER_TICKGROUP_CREATED, r4);

    // Case 5: success path assigns API and group id
    g_timerApi[TIMER_TYPE_SWTMR].createTimer = NULL;
    g_timerApi[TIMER_TYPE_SWTMR].startTimer = NULL;
    g_timerApi[TIMER_TYPE_SWTMR].stopTimer = NULL;
    g_timerApi[TIMER_TYPE_SWTMR].deleteTimer = NULL;
    g_timerApi[TIMER_TYPE_SWTMR].restartTimer = NULL;
    g_timerApi[TIMER_TYPE_SWTMR].timerQuery = NULL;
    g_timerApi[TIMER_TYPE_SWTMR].getOverrun = NULL;
    cfg.maxTimerNum = 4;
    gid = 0;
    U32 r5 = OsSwTimerGroupCreate(&cfg, &gid);
    int apiSet = (g_timerApi[TIMER_TYPE_SWTMR].createTimer != NULL) && (g_timerApi[TIMER_TYPE_SWTMR].startTimer != NULL) &&
                 (g_timerApi[TIMER_TYPE_SWTMR].stopTimer != NULL) && (g_timerApi[TIMER_TYPE_SWTMR].deleteTimer != NULL) &&
                 (g_timerApi[TIMER_TYPE_SWTMR].restartTimer != NULL) && (g_timerApi[TIMER_TYPE_SWTMR].timerQuery != NULL) &&
                 (g_timerApi[TIMER_TYPE_SWTMR].getOverrun != NULL);
    TU_check_u32("success ret", OS_OK, r5);
    TU_check_u32("group id", OS_TICK_SWTMR_GROUP_ID, gid);
    TU_check_bool("api function table set", 1, apiSet);

    TU_report();
    return 0;
}

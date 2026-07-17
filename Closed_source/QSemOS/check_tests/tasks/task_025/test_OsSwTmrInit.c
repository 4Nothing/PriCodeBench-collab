#define SKIP_OsSwTmrInit
#include "prt_swtmr_internal.h"
#include "test_util.h"

// Simplified OsSwTimerGroupCreate for testing OsSwTmrInit
static U32 OsSwTimerGroupCreate(struct TmrGrpUserCfg *config, TimerGroupId *groupId)
{
    if (g_tickModInfo.tickPerSecond == 0) {
        return OS_ERRNO_TICK_NOT_INIT;
    }

    if (config->maxTimerNum == 0) {
        return OS_ERRNO_TIMER_NUM_ZERO;
    }

    if (config->maxTimerNum > (U32)OS_MAX_U16) {
        return OS_ERRNO_TIMER_NUM_TOO_LARGE;
    }

    if (g_timerApi[TIMER_TYPE_SWTMR].createTimer != NULL) {
        return OS_ERRNO_TIMER_TICKGROUP_CREATED;
    }

    g_swTmrMaxNum = config->maxTimerNum;
    *groupId = OS_TICK_SWTMR_GROUP_ID;
    g_timerApi[TIMER_TYPE_SWTMR].createTimer = (TimerCreateFunc)1; // mark as created

    return OS_OK;
}

#include "func_under_test.c"

int main(void)
{
    TU_reset();

    // Ensure environment clean
    g_timerApi[0].createTimer = NULL;
    g_tickModInfo.tickPerSecond = 1000;

    // Case 1: success with 4 timers
    U32 r1 = OsSwTmrInit(4);
    TU_check_u32("init ret", OS_OK, r1);
    TU_check_u32("group id set", OS_TICK_SWTMR_GROUP_ID, g_tickSwTmrGroupId);

    // Case 2: calling again should say already created
    U32 r2 = OsSwTmrInit(4);
    TU_check_u32("already created", OS_ERRNO_TIMER_TICKGROUP_CREATED, r2);

    // Case 3: tick not init path
    g_timerApi[0].createTimer = NULL; // reset created flag
    g_tickModInfo.tickPerSecond = 0;
    U32 r3 = OsSwTmrInit(2);
    TU_check_u32("tick not init", OS_ERRNO_TICK_NOT_INIT, r3);

    // Case 4: zero timers
    g_tickModInfo.tickPerSecond = 1000;
    g_timerApi[0].createTimer = NULL;
    U32 r4 = OsSwTmrInit(0);
    TU_check_u32("zero timers", OS_ERRNO_TIMER_NUM_ZERO, r4);

    // Case 5: too many timers
    U32 r5 = OsSwTmrInit((U32)OS_MAX_U16 + 1);
    TU_check_u32("too many timers", OS_ERRNO_TIMER_NUM_TOO_LARGE, r5);

    TU_report();
    return 0;
}

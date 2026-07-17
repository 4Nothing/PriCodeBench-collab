#define SKIP_OsSwTmrCreateTimer
#define SKIP_OsSwTmrCreateTimerParaChk
#include "prt_swtmr_internal.h"
#include "test_util.h"

static U32 OsSwTmrCreateTimerParaChk(struct TimerCreatePara *createPara)
{
    if (createPara->callBackFunc == NULL) {
        return OS_ERRNO_TIMER_PROC_FUNC_NULL;
    }
    if (createPara->timerGroupId != OS_TICK_SWTMR_GROUP_ID) {
        return OS_ERRNO_TIMERGROUP_ID_INVALID;
    }
    if (g_tickModInfo.tickPerSecond == 0) {
        return OS_ERRNO_TICK_NOT_INIT;
    }
    U64 ticks64 = (U64)g_tickModInfo.tickPerSecond * (U64)createPara->interval;
    if (ticks64 % OS_SYS_MS_PER_SECOND != 0) {
        return OS_ERRNO_SWTMR_INTERVAL_NOT_SUITED;
    }
    U64 maxInterval = ((U64)OS_MAX_U32 * (U64)OS_SYS_MS_PER_SECOND) / (U64)g_tickModInfo.tickPerSecond;
    if ((U64)createPara->interval > maxInterval) {
        return OS_ERRNO_SWTMR_INTERVAL_OVERFLOW;
    }
    return OS_OK;
}

static U32 OsSwTmrCreateTimer(struct TimerCreatePara *createPara, TimerHandle *tmrHandle)
{
    struct TagSwTmrCtrl *swtmr = NULL;
    U32 ret;

    ret = OsSwTmrCreateTimerParaChk(createPara);
    if (ret != OS_OK) {
        return ret;
    }

    if (g_tmrFreeList.freeList == NULL) {
        return OS_ERRNO_SWTMR_MAXSIZE;
    }

    swtmr = g_tmrFreeList.freeList;
    g_tmrFreeList.freeList = swtmr->next;

    *tmrHandle = OS_SWTMR_INDEX_2_ID(swtmr->swtmrIndex);

    return OS_OK;
}

static void cb(TimerHandle a1, U32 a2, U32 a3, U32 a4, U32 a5)
{
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5;
}

int main(void)
{
    TU_reset();
    g_tickModInfo.tickPerSecond = 200; // non 1kHz
    static struct TagSwTmrCtrl node;
    node.swtmrIndex = 7;
    node.next = NULL;
    node.state = OS_TIMER_FREE;
    g_tmrFreeList.freeList = &node;

    struct TimerCreatePara p = {0};
    p.callBackFunc = cb;
    p.mode = 3;
    p.interval = 500;
    p.timerGroupId = OS_TICK_SWTMR_GROUP_ID;

    TimerHandle h = 0;
    U32 r1 = OsSwTmrCreateTimer(&p, &h);
    TU_check_u32("create ok", OS_OK, r1);
    TU_check_u32("handle=0x10000007", OS_SWTMR_INDEX_2_ID(7), h);

    // Exhausted now
    U32 r2 = OsSwTmrCreateTimer(&p, &h);
    TU_check_u32("exhausted", OS_ERRNO_SWTMR_MAXSIZE, r2);

    // Different interval ms but still integer tick at 200Hz
    p.interval = 1000;     // 1 second => 200 ticks
    g_tmrFreeList.freeList = &node; // reset
    node.next = NULL;
    U32 r3 = OsSwTmrCreateTimer(&p, &h);
    TU_check_u32("create ok 2", OS_OK, r3);

    // bad interval not suited when 200Hz and 7ms -> 1.4 ticks remainder
    p.interval = 7;
    U32 r4 = OsSwTmrCreateTimer(&p, &h);
    TU_check_u32("interval not suited", OS_ERRNO_SWTMR_INTERVAL_NOT_SUITED, r4);

    // overflow detection - use tickPerSecond=1000000, maxInterval=(OS_MAX_U32*1000)/1000000=4294967
    g_tickModInfo.tickPerSecond = 1000000;
    g_tmrFreeList.freeList = &node; // reset
    node.next = NULL;
    p.interval = 5000000; // > 4294967 and divisible by 1000
    U32 r5 = OsSwTmrCreateTimer(&p, &h);
    TU_check_u32("overflow", OS_ERRNO_SWTMR_INTERVAL_OVERFLOW, r5);

    TU_report();
    return 0;
}

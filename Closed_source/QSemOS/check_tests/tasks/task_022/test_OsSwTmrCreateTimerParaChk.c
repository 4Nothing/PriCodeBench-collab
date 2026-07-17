#define SKIP_OsSwTmrCreateTimerParaChk
#include "prt_swtmr_internal.h"
#include "test_util.h"
#include "func_under_test.c"

static void dummy_cb(TimerHandle a1, U32 a2, U32 a3, U32 a4, U32 a5)
{
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5;
}

int main(void)
{
    TU_reset();

    struct TimerCreatePara p = {0};

    // Case 1: callback null
    p.callBackFunc = NULL;
    p.timerGroupId = OS_TICK_SWTMR_GROUP_ID;
    p.interval = 1000; // 1s
    TU_check_u32("cb null", OS_ERRNO_TIMER_PROC_FUNC_NULL, OsSwTmrCreateTimerParaChk(&p));

    // Case 2: wrong group id
    p.callBackFunc = dummy_cb;
    p.timerGroupId = 99;
    p.interval = 1000;
    TU_check_u32("wrong group id", OS_ERRNO_TIMERGROUP_ID_INVALID, OsSwTmrCreateTimerParaChk(&p));

    // Case 3: interval not suited (tickPerSecond=1000, ms to tick must be integer)
    p.timerGroupId = OS_TICK_SWTMR_GROUP_ID;
    p.interval = 333;                   // 333 ms -> 333 ticks ok. Use 333.3 impossible in int
    g_tickModInfo.tickPerSecond = 1024; // 1024 Hz, 333 ms -> 340. - remainder exists
    TU_check_u32("interval not suited", OS_ERRNO_SWTMR_INTERVAL_NOT_SUITED, OsSwTmrCreateTimerParaChk(&p));

    // Case 4: overflow
    g_tickModInfo.tickPerSecond = 1000; // back to 1kHz
    p.interval = (U32)OS_MAX_U32;       // check <= OK first
    TU_check_u32("overflow boundary ok", OS_OK, OsSwTmrCreateTimerParaChk(&p));
    p.interval = (U32)OS_MAX_U32 - 1; // also OK
    TU_check_u32("large ok", OS_OK, OsSwTmrCreateTimerParaChk(&p));
    // force overflow by increasing tickPerSecond so boundary becomes smaller than OS_MAX_U32
    // boundary = (1000*OS_MAX_U32)/tickPerSecond. With 2000 Hz, boundary = OS_MAX_U32/2
    g_tickModInfo.tickPerSecond = 2000;
    p.interval = (U32)OS_MAX_U32; // remainder check passes because 2000%1000==0
    TU_check_u32("overflow detected", OS_ERRNO_SWTMR_INTERVAL_OVERFLOW, OsSwTmrCreateTimerParaChk(&p));

    // Case 5: success
    g_tickModInfo.tickPerSecond = 1000;
    p.interval = 500; // 500 ms -> 500 ticks
    TU_check_u32("success", OS_OK, OsSwTmrCreateTimerParaChk(&p));

    TU_report();
    return 0;
}

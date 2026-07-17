#define SKIP_OsSwTmrCreateTimerCbInit
#include "prt_swtmr_internal.h"
#include "test_util.h"
#include "func_under_test.c"

static void cb(TimerHandle a1, U32 a2, U32 a3, U32 a4, U32 a5)
{
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5;
}

int main(void)
{
    TU_reset();
    struct TimerCreatePara p = {0};
    p.callBackFunc = cb;
    p.mode = 2;
    p.interval = 250;
    p.timerGroupId = OS_TICK_SWTMR_GROUP_ID;
    p.arg1 = 1;
    p.arg2 = 2;
    p.arg3 = 3;
    p.arg4 = 4;
    struct TagSwTmrCtrl t = {0};

    OsSwTmrCreateTimerCbInit(&p, &t, 25);

    TU_check_ptr("handler", (void *)cb, (void *)t.handler);
    TU_check_u32("mode", 2, t.mode);
    TU_check_u32("interval", 25, t.interval);
    TU_check_u32("idxRollNum", 25, t.idxRollNum);
    TU_check_u32("overrun", 0, t.overrun);
    TU_check_u32("state", OS_TIMER_CREATED, t.state);
    TU_check_u32("arg1", 1, (U32)t.arg1);

    TU_report();
    return 0;
}

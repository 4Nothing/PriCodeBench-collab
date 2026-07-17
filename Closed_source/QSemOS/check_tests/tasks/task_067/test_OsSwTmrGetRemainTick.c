#include <stdio.h>
#include "../include/prt_swtmr_internal.h"
#include "test_support.h"
#include "../task_066/func_under_test.c"
#include "func_under_test.c"

int main(void)
{
    Test_InitWheel();
    Test_ResetGlobals();
    struct TestStats st;
    Test_InitStats(&st);

    // Case 1: state CREATED => returns idxRollNum
    struct TagSwTmrCtrl t1 = {0};
    t1.state = OS_TIMER_CREATED;
    t1.idxRollNum = 123;
    U32 r = OsSwTmrGetRemainTick(&t1);
    Test_CheckEqU32("CREATED returns idx", 123, r, &st);

    // Case 2: state EXPIRED => returns 0
    struct TagSwTmrCtrl t2 = {0};
    t2.state = OS_TIMER_EXPIRED;
    r = OsSwTmrGetRemainTick(&t2);
    Test_CheckEqU32("EXPIRED returns 0", 0, r, &st);

    // Case 3: state RUNNING w/ cursor 0, ticks 50 => 50
    struct TagSwTmrCtrl t3 = {0};
    Test_SetCursor(0);
    OsSwTmrStart(&t3, 50);
    r = OsSwTmrGetRemainTick(&t3);
    Test_CheckEqU32("RUNNING cur0 t50", 50, r, &st);

    // Case 4: cursor=200%64=8, ticks=10, idx=10 => remain=2
    struct TagSwTmrCtrl t4 = {0};
    Test_SetCursor(200);
    OsSwTmrStart(&t4, 10);
    r = OsSwTmrGetRemainTick(&t4);
    Test_CheckEqU32("RUN wrap 200->10", (U32)((10 % OS_SWTMR_SORTLINK_LEN) - (200 % OS_SWTMR_SORTLINK_LEN)), r, &st);

    // Case 5: state RUNNING w/ one full roll 266 and cur 0 => 266
    struct TagSwTmrCtrl t5 = {0};
    Test_SetCursor(0);
    OsSwTmrStart(&t5, OS_SWTMR_SORTLINK_LEN + 10);
    r = OsSwTmrGetRemainTick(&t5);
    Test_CheckEqU32("RUN roll+10", OS_SWTMR_SORTLINK_LEN + 10, r, &st);

    Test_Summary("OsSwTmrGetRemainTick", &st);
    return (st.pass == st.total) ? 0 : 1;
}

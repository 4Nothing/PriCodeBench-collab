#include <stdio.h>
#include <string.h>
#include "../include/prt_swtmr_internal.h"
#include "test_support.h"
#include "../task_066/func_under_test.c"
#include "../task_067/func_under_test.c"
#include "func_under_test.c"

int main(void)
{
    Test_InitWheel();
    Test_ResetGlobals();
    struct TestStats st;
    Test_InitStats(&st);

    struct TagSwTmrCtrl arr[2];
    memset(arr, 0, sizeof(arr));
    Test_BindTimerArray(arr, 2);
    Test_SetTickPerSecond(1000); // 1 tick == 1 ms for simplicity

    U32 ms;

    // Case 1: invalid handle
    U32 ret = OsSwTmrQuery(OS_SWTMR_INDEX_2_ID(2), &ms);
    Test_CheckEqU32("invalid handle", OS_ERRNO_TIMER_HANDLE_INVALID, ret, &st);

    // Case 2: FREE -> not created
    arr[0].state = OS_TIMER_FREE;
    ret = OsSwTmrQuery(OS_SWTMR_INDEX_2_ID(0), &ms);
    Test_CheckEqU32("free -> not created", OS_ERRNO_SWTMR_NOT_CREATED, ret, &st);

    // Case 3: CREATED with idxRollNum 123 -> 123ms
    arr[0].state = OS_TIMER_CREATED;
    arr[0].idxRollNum = 123;
    ret = OsSwTmrQuery(OS_SWTMR_INDEX_2_ID(0), &ms);
    Test_CheckEqU32("created -> ok", OS_OK, ret, &st);
    Test_CheckEqU32("ms=123", 123, ms, &st);

    // Case 4: RUNNING at 50 ticks remaining -> 50ms
    arr[1].state = OS_TIMER_CREATED;
    Test_SetCursor(0);
    OsSwTmrStart(&arr[1], 50);
    ret = OsSwTmrQuery(OS_SWTMR_INDEX_2_ID(1), &ms);
    Test_CheckEqU32("running -> ok", OS_OK, ret, &st);
    Test_CheckEqU32("ms=50", 50, ms, &st);

    // Case 5: Fractional rounding up: tickPerSecond=3; remainTick=1000/3 + remainder
    Test_SetTickPerSecond(3);
    struct TagSwTmrCtrl t = {0};
    Test_BindTimerArray(&t, 1);
    t.state = OS_TIMER_CREATED;
    t.idxRollNum = 2; // 2 ticks -> (2*1000)/3 = 666 remainder 2 -> should +1 => 667
    ret = OsSwTmrQuery(OS_SWTMR_INDEX_2_ID(0), &ms);
    Test_CheckEqU32("round up", OS_OK, ret, &st);
    Test_CheckEqU32("ms=667", 667, ms, &st);

    Test_Summary("OsSwTmrQuery", &st);
    return (st.pass == st.total) ? 0 : 1;
}

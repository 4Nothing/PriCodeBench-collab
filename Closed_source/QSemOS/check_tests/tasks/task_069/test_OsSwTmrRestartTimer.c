#include <stdio.h>
#include <string.h>
#include "../include/prt_swtmr_internal.h"
#include "test_support.h"
#include "../task_066/func_under_test.c"
#include "../task_067/func_under_test.c"
#include "../task_071/func_under_test.c"
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

    // Case 1: invalid handle
    U32 ret = OsSwTmrRestartTimer(OS_SWTMR_INDEX_2_ID(2));
    Test_CheckEqU32("invalid handle", OS_ERRNO_TIMER_HANDLE_INVALID, ret, &st);

    // Case 2: FREE -> not created
    arr[0].state = OS_TIMER_FREE;
    ret = OsSwTmrRestartTimer(OS_SWTMR_INDEX_2_ID(0));
    Test_CheckEqU32("free -> not created", OS_ERRNO_SWTMR_NOT_CREATED, ret, &st);

    // Case 3: EXPIRED -> pre running|expired and OK
    arr[0].state = OS_TIMER_EXPIRED;
    ret = OsSwTmrRestartTimer(OS_SWTMR_INDEX_2_ID(0));
    Test_CheckEqU32("expired -> OK", OS_OK, ret, &st);
    Test_CheckEqU8("state pre running|expired", (U8)(OS_SWTMR_PRE_RUNNING | OS_TIMER_EXPIRED), arr[0].state, &st);

    // Case 4: RUNNING -> stop then restart with interval
    arr[1].state = OS_TIMER_CREATED;
    arr[1].interval = 42;
    OsSwTmrStart(&arr[1], 10); // make running; idxRollNum=10
    ret = OsSwTmrRestartTimer(OS_SWTMR_INDEX_2_ID(1));
    Test_CheckEqU32("running restart OK", OS_OK, ret, &st);
    Test_CheckEqU32("idx set to interval", (U32)(42u << 26), arr[1].idxRollNum, &st);
    Test_CheckEqU8("state running", OS_TIMER_RUNNING, arr[1].state, &st);

    // Case 5: CREATED -> start with interval
    struct TagSwTmrCtrl t = {0};
    Test_BindTimerArray(&t, 1);
    t.state = OS_TIMER_CREATED;
    t.interval = 77;
    ret = OsSwTmrRestartTimer(OS_SWTMR_INDEX_2_ID(0));
    Test_CheckEqU32("created restart OK", OS_OK, ret, &st);
    Test_CheckEqU32("idx=interval", (U32)((U32)(77 % OS_SWTMR_SORTLINK_LEN) << 26) | (U32)(77 / OS_SWTMR_SORTLINK_LEN), t.idxRollNum, &st);
    Test_CheckEqU8("state running", OS_TIMER_RUNNING, t.state, &st);

    Test_Summary("OsSwTmrRestartTimer", &st);
    return (st.pass == st.total) ? 0 : 1;
}

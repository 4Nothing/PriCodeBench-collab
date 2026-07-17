#include <stdio.h>
#include <string.h>
#include "../include/prt_swtmr_internal.h"
#include "test_support.h"
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

    // Case 1: invalid handle index >= g_swTmrMaxNum
    U32 ret = OsSwTmrStartTimer(OS_SWTMR_INDEX_2_ID(2));
    Test_CheckEqU32("invalid handle", OS_ERRNO_TIMER_HANDLE_INVALID, ret, &st);

    // Case 2: state FREE -> not created error
    arr[0].state = OS_TIMER_FREE;
    ret = OsSwTmrStartTimer(OS_SWTMR_INDEX_2_ID(0));
    Test_CheckEqU32("free -> not created", OS_ERRNO_SWTMR_NOT_CREATED, ret, &st);

    // Case 3: state EXPIRED -> PRE_RUNNING|EXPIRED and OK
    arr[0].state = OS_TIMER_EXPIRED;
    ret = OsSwTmrStartTimer(OS_SWTMR_INDEX_2_ID(0));
    Test_CheckEqU32("expired -> OK", OS_OK, ret, &st);
    Test_CheckEqU8("state pre running|expired", (U8)(OS_SWTMR_PRE_RUNNING | OS_TIMER_EXPIRED), arr[0].state, &st);

    // Case 4: state RUNNING -> OK, unchanged
    arr[0].state = OS_TIMER_RUNNING;
    ret = OsSwTmrStartTimer(OS_SWTMR_INDEX_2_ID(0));
    Test_CheckEqU32("running -> OK", OS_OK, ret, &st);
    Test_CheckEqU8("state running", OS_TIMER_RUNNING, arr[0].state, &st);

    // Case 5: state CREATED with idxRollNum==0 -> use interval
    arr[1].state = OS_TIMER_CREATED;
    arr[1].interval = 77;
    arr[1].idxRollNum = 0;
    ret = OsSwTmrStartTimer(OS_SWTMR_INDEX_2_ID(1));
    Test_CheckEqU32("created start ok", OS_OK, ret, &st);
    Test_CheckEqU32("idx set to interval", (U32)((U32)(77 % OS_SWTMR_SORTLINK_LEN) << 26) | (U32)(77 / OS_SWTMR_SORTLINK_LEN), arr[1].idxRollNum, &st);
    Test_CheckEqU8("state running after start", OS_TIMER_RUNNING, arr[1].state, &st);

    Test_Summary("OsSwTmrStartTimer", &st);
    return (st.pass == st.total) ? 0 : 1;
}

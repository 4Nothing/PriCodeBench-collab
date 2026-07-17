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
    U32 ret = OsSwTmrStopTimer(OS_SWTMR_INDEX_2_ID(2));
    Test_CheckEqU32("invalid handle", OS_ERRNO_TIMER_HANDLE_INVALID, ret, &st);

    // Case 2: FREE -> not created
    arr[0].state = OS_TIMER_FREE;
    ret = OsSwTmrStopTimer(OS_SWTMR_INDEX_2_ID(0));
    Test_CheckEqU32("free -> not created", OS_ERRNO_SWTMR_NOT_CREATED, ret, &st);

    // Case 3: EXPIRED -> set PRE_CREATED|EXPIRED and OK
    arr[0].state = OS_TIMER_EXPIRED;
    ret = OsSwTmrStopTimer(OS_SWTMR_INDEX_2_ID(0));
    Test_CheckEqU32("expired -> OK", OS_OK, ret, &st);
    Test_CheckEqU8("state pre created|expired", (U8)(OS_SWTMR_PRE_CREATED | OS_TIMER_EXPIRED), arr[0].state, &st);

    // Case 4: CREATED -> unstart error
    arr[0].state = OS_TIMER_CREATED;
    ret = OsSwTmrStopTimer(OS_SWTMR_INDEX_2_ID(0));
    Test_CheckEqU32("created -> unstart", OS_ERRNO_SWTMR_UNSTART, ret, &st);

    // Case 5: RUNNING -> stop and OK, state becomes CREATED
    arr[1].state = OS_TIMER_CREATED;
    // put arr[1] into a slot by starting via helper (imitate running)
    OsSwTmrStart(&arr[1], 30);
    ret = OsSwTmrStopTimer(OS_SWTMR_INDEX_2_ID(1));
    Test_CheckEqU32("running stop OK", OS_OK, ret, &st);
    Test_CheckEqU8("state created", OS_TIMER_CREATED, arr[1].state, &st);

    Test_Summary("OsSwTmrStopTimer", &st);
    return (st.pass == st.total) ? 0 : 1;
}

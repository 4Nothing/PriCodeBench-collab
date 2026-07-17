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

    U32 ov = 0;
    U32 ret;

    // Case 1: invalid handle
    ret = OsSwTmrGetOverrun(OS_SWTMR_INDEX_2_ID(2), &ov);
    Test_CheckEqU32("invalid handle", OS_ERRNO_TIMER_HANDLE_INVALID, ret, &st);

    // Case 2: zero overrun
    arr[0].overrun = 0;
    ret = OsSwTmrGetOverrun(OS_SWTMR_INDEX_2_ID(0), &ov);
    Test_CheckEqU32("ok zero", OS_OK, ret, &st);
    Test_CheckEqU32("ov=0", 0, ov, &st);

    // Case 3: small overrun
    arr[0].overrun = 3;
    ret = OsSwTmrGetOverrun(OS_SWTMR_INDEX_2_ID(0), &ov);
    Test_CheckEqU32("ok small", OS_OK, ret, &st);
    Test_CheckEqU32("ov=3", 3, ov, &st);

    // Case 4: large overrun (max U8 value is 255)
    arr[1].overrun = 255;
    ret = OsSwTmrGetOverrun(OS_SWTMR_INDEX_2_ID(1), &ov);
    Test_CheckEqU32("ok large", OS_OK, ret, &st);
    Test_CheckEqU32("ov=255", 255, ov, &st);

    // Case 5: mutation check independent of state
    arr[1].state = OS_TIMER_FREE;
    arr[1].overrun = 7;
    ret = OsSwTmrGetOverrun(OS_SWTMR_INDEX_2_ID(1), &ov);
    Test_CheckEqU32("state ignored", OS_OK, ret, &st);
    Test_CheckEqU32("ov=7", 7, ov, &st);

    Test_Summary("OsSwTmrGetOverrun", &st);
    return (st.pass == st.total) ? 0 : 1;
}

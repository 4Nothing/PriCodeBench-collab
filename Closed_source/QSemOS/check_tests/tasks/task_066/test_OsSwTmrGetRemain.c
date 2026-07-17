#include <stdio.h>
#include "../include/prt_swtmr_internal.h"
#include "test_support.h"
#include "func_under_test.c"

int main(void)
{
    struct TestStats st;
    Test_InitStats(&st);
    Test_InitWheel();
    Test_ResetGlobals();

    struct TagSwTmrCtrl t1 = {0};
    // Place t1 in slot of idx 50
    OsSwTmrStart(&t1, 50);

    // Case 1: cursor=0 => remain = 50
    Test_SetCursor(0);
    U32 r = OsSwTmrGetRemain(&t1);
    Test_CheckEqU32("cursor=0, idx=50", 50, r, &st);

    // Case 2: cursor=49 => remain = 1
    Test_SetCursor(49);
    r = OsSwTmrGetRemain(&t1);
    Test_CheckEqU32("cursor=49, idx=50", 1, r, &st);

    // Case 3: cursor=50 => remain = 256 (wrap) - 0 => 256
    Test_SetCursor(50);
    r = OsSwTmrGetRemain(&t1);
    Test_CheckEqU32("cursor=50, idx=50", OS_SWTMR_SORTLINK_LEN, r, &st);

    // Case 4: cursor=60 => remain = 246
    Test_SetCursor(60);
    r = OsSwTmrGetRemain(&t1);
    Test_CheckEqU32("cursor=60, idx=50", OS_SWTMR_SORTLINK_LEN - 10, r, &st);

    // Case 5: idxRollNum with rollnum>0: ticks=266 (1 full roll + 10) and cursor=0 => 266
    struct TagSwTmrCtrl t2 = {0};
    OsSwTmrStart(&t2, OS_SWTMR_SORTLINK_LEN + 10);
    Test_SetCursor(0);
    r = OsSwTmrGetRemain(&t2);
    Test_CheckEqU32("roll=1, idx=10, cur=0", OS_SWTMR_SORTLINK_LEN + 10, r, &st);

    Test_Summary("OsSwTmrGetRemain", &st);
    return (st.pass == st.total) ? 0 : 1;
}

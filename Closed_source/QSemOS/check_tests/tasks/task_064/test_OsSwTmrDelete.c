#include <stdio.h>
#include "../include/prt_swtmr_internal.h"
#include "test_support.h"
#include "func_under_test.c"

int main(void)
{
    Test_InitWheel();
    Test_ResetGlobals();
    struct TestStats st;
    Test_InitStats(&st);

    // Case 1: delete moves to free list and sets state FREE
    struct TagSwTmrCtrl t1 = {0};
    OsSwTmrDelete(&t1);
    Test_CheckEqPtr("free head is t1", &t1, g_tmrFreeList, &st);
    Test_CheckEqU8("state FREE", OS_TIMER_FREE, t1.state, &st);

    // Case 2: multiple deletes create stack-like list
    struct TagSwTmrCtrl t2 = {0};
    OsSwTmrDelete(&t2);
    Test_CheckEqPtr("new head t2", &t2, g_tmrFreeList, &st);
    Test_CheckEqPtr("t2.next -> t1", &t1, t2.next, &st);

    // Case 3: delete of timer with existing links should overwrite next only
    struct TagSwTmrCtrl t3 = {0};
    struct TagSwTmrCtrl t4 = {0};
    // link t3<->t4 artificially
    t3.next = &t4;
    t4.prev = &t3;
    OsSwTmrDelete(&t3);
    Test_CheckEqPtr("head t3", &t3, g_tmrFreeList, &st);
    Test_CheckEqU8("t3 FREE", OS_TIMER_FREE, t3.state, &st);

    // Case 4: state should be FREE regardless of previous value
    t4.state = OS_TIMER_CREATED;
    OsSwTmrDelete(&t4);
    Test_CheckEqU8("t4 FREE", OS_TIMER_FREE, t4.state, &st);

    // Case 5: deleting NULL is undefined; we skip. Instead, check list chaining continues
    struct TagSwTmrCtrl t5 = {0};
    OsSwTmrDelete(&t5);
    Test_CheckEqPtr("head t5", &t5, g_tmrFreeList, &st);

    Test_Summary("OsSwTmrDelete", &st);
    return (st.pass == st.total) ? 0 : 1;
}

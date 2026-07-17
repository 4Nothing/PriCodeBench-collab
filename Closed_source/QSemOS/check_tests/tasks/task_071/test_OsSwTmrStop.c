#include <stdio.h>
#include "../include/prt_swtmr_internal.h"
#include "test_support.h"
#include "../task_066/func_under_test.c"
#include "../task_067/func_under_test.c"
#include "func_under_test.c"

static void setup_three_in_slot(struct TagSwTmrCtrl *a, struct TagSwTmrCtrl *b, struct TagSwTmrCtrl *c, U32 base)
{
    // Insert a,b,c in same slot index base to form head<->a<->b<->c<->head
    Test_SetCursor(0);
    OsSwTmrStart(a, base);
    OsSwTmrStart(b, base);
    OsSwTmrStart(c, base);
}

int main(void)
{
    Test_InitWheel();
    Test_ResetGlobals();
    struct TestStats st;
    Test_InitStats(&st);

    // Case 1: Stop single timer, reckonOff=false -> idxRollNum updated to remain
    struct TagSwTmrCtrl t1 = {0};
    Test_SetCursor(10);
    OsSwTmrStart(&t1, 50); // remain should be (50-10)=40 (no wrap)
    OsSwTmrStop(&t1, FALSE);
    Test_CheckEqU32("stop set remain", 40, t1.idxRollNum, &st);
    Test_CheckEqU8("state CREATED", OS_TIMER_CREATED, t1.state, &st);

    // Case 2: Stop with reckonOff=true -> idxRollNum not recomputed, remains same as before stop (50)
    struct TagSwTmrCtrl t2 = {0};
    Test_SetCursor(0);
    OsSwTmrStart(&t2, 50);
    OsSwTmrStop(&t2, TRUE);
    Test_CheckEqU32("stop keep idx", (U32)(50u << 26), t2.idxRollNum, &st);

    // Case 3: Removing middle node should link neighbors
    struct TagSwTmrCtrl a = {0}, b = {0}, c = {0};
    setup_three_in_slot(&a, &b, &c, 20);
    // Remove b
    OsSwTmrStop(&b, TRUE);
    // Insert order was a, b, c -> list is head->c->b->a->head, after removing b: head->c->a->head
    Test_CheckEqPtr("unlink mid->c.next==a", &a, c.next, &st);
    Test_CheckEqPtr("unlink mid->a.prev==c", &c, a.prev, &st);

    // Case 4: Remove head-adjacent node (first element) from its slot list
    struct TagSwTmrCtrl x = {0}, y = {0};
    Test_SetCursor(0);
    OsSwTmrStart(&x, 5);
    OsSwTmrStart(&y, 5);
    // y is at front now; stop y
    OsSwTmrStop(&y, TRUE);
    Test_CheckEqPtr("first removed -> head next x", &x, (g_tmrSortLink.sortLink + 5)->next, &st);

    // Case 5: After stop, pointers are nulled
    Test_CheckEqPtr("prev NULL", NULL, y.prev, &st);
    Test_CheckEqPtr("next NULL", NULL, y.next, &st);

    Test_Summary("OsSwTmrStop", &st);
    return (st.pass == st.total) ? 0 : 1;
}

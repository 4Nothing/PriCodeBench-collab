#define SKIP_OsSwtmrTargetCheck
#include "prt_swtmr_internal.h"
#include "test_util.h"
#include "func_under_test.c"

int main(void)
{
    TU_reset();

    static struct TagSwTmrCtrl arr[3];
    g_swtmrCbArray = arr;
    for (int i = 0; i < 3; ++i)
    {
        arr[i].swtmrIndex = (U16)i;
        arr[i].state = OS_TIMER_CREATED;
    }
    g_swTmrMaxNum = 3;

    // Case 1: none active
    bool b1 = OsSwtmrTargetCheck(0);
    TU_check_bool("none active -> false", 0, b1);

    // Case 2: one running
    arr[1].state = OS_TIMER_RUNNING;
    bool b2 = OsSwtmrTargetCheck(0);
    TU_check_bool("one running -> true", 1, b2);

    // Case 3: expired also counts
    arr[1].state = OS_TIMER_CREATED;
    arr[2].state = OS_TIMER_EXPIRED;
    bool b3 = OsSwtmrTargetCheck(0);
    TU_check_bool("expired -> true", 1, b3);

    // Case 4: free ignored
    for (int i = 0; i < 3; ++i)
        arr[i].state = OS_TIMER_FREE;
    bool b4 = OsSwtmrTargetCheck(0);
    TU_check_bool("all free -> false", 0, b4);

    // Case 5: created ignored, mix
    arr[0].state = OS_TIMER_CREATED;
    arr[1].state = OS_TIMER_FREE;
    arr[2].state = OS_TIMER_CREATED;
    bool b5 = OsSwtmrTargetCheck(0);
    TU_check_bool("created ignored -> false", 0, b5);

    TU_report();
    return 0;
}

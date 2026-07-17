#define SKIP_OsSwTmrDeleteTimer
#include "prt_swtmr_internal.h"
#include "test_util.h"
#include "func_under_test.c"

int main(void)
{
    TU_reset();

    static struct TagSwTmrCtrl arr[3];
    g_swtmrCbArray = arr;
    arr[0].swtmrIndex = 0;
    arr[1].swtmrIndex = 1;
    arr[2].swtmrIndex = 2;

    // Case 1: invalid handle index
    TimerHandle h = 5; // >= g_swTmrMaxNum
    U32 r1 = OsSwTmrDeleteTimer(h);
    TU_check_u32("handle invalid", OS_ERRNO_TIMER_HANDLE_INVALID, r1);

    g_swTmrMaxNum = 3;

    // Case 2: not created (FREE)
    arr[0].state = OS_TIMER_FREE;
    U32 r2 = OsSwTmrDeleteTimer(OS_SWTMR_INDEX_2_ID(0));
    TU_check_u32("not created", OS_ERRNO_SWTMR_NOT_CREATED, r2);

    // Case 3: expired -> set pre_free and return OK
    arr[1].state = OS_TIMER_EXPIRED;
    U32 r3 = OsSwTmrDeleteTimer(OS_SWTMR_INDEX_2_ID(1));
    TU_check_u32("expired pre_free", OS_OK, r3);
    TU_check_u32("state pre_free|expired", (OS_SWTMR_PRE_FREE | OS_TIMER_EXPIRED), arr[1].state);

    // Case 4: running -> stop then delete
    arr[2].state = OS_TIMER_RUNNING;
    U32 r4 = OsSwTmrDeleteTimer(OS_SWTMR_INDEX_2_ID(2));
    TU_check_u32("running delete", OS_OK, r4);
    TU_check_u32("state free after delete", OS_TIMER_FREE, arr[2].state);

    // Case 5: created -> delete
    arr[0].state = OS_TIMER_CREATED;
    U32 r5 = OsSwTmrDeleteTimer(OS_SWTMR_INDEX_2_ID(0));
    TU_check_u32("created delete", OS_OK, r5);
    TU_check_u32("state free after", OS_TIMER_FREE, arr[0].state);

    TU_report();
    return 0;
}

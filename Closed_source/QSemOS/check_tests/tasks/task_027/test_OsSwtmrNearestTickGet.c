#define SKIP_OsSwtmrNearestTickGet
#include "prt_swtmr_internal.h"
#include "test_util.h"
#include "func_under_test.c"

int main(void)
{
    TU_reset();

    static struct TagSwTmrCtrl arr[4];
    g_swtmrCbArray = arr;
    for (int i = 0; i < 4; ++i)
    {
        arr[i].swtmrIndex = (U16)i;
        arr[i].state = OS_TIMER_CREATED;
    }
    g_swTmrMaxNum = 4;
    g_uniTicks = 1000; // now

    // Case 1: no active timers (all FREE or CREATED)
    for (int i = 0; i < 4; ++i)
    {
        arr[i].state = OS_TIMER_CREATED;
    }
    U64 t1 = OsSwtmrNearestTickGet(0);
    TU_check_u64("none active -> forever", (unsigned long long)OS_TICKLESS_FOREVER, (unsigned long long)t1);

    // Case 2: one running with expectedTick in future
    arr[2].state = OS_TIMER_RUNNING;
    arr[2].expectedTick = 1500;
    U64 t2 = OsSwtmrNearestTickGet(0);
    TU_check_u64("nearest 1500", 1500ULL, t2);

    // Case 3: multiple choose smallest >= now
    arr[1].state = OS_TIMER_RUNNING;
    arr[1].expectedTick = 1200;
    arr[3].state = OS_TIMER_RUNNING;
    arr[3].expectedTick = 2000;
    U64 t3 = OsSwtmrNearestTickGet(0);
    TU_check_u64("nearest 1200", 1200ULL, t3);

    // Case 4: expectedTick < now should be ignored per condition (g_uniTicks <= expectedTick)
    arr[0].state = OS_TIMER_RUNNING;
    arr[0].expectedTick = 900; // less than now
    U64 t4 = OsSwtmrNearestTickGet(0);
    TU_check_u64("still 1200", 1200ULL, t4);

    // Case 5: if all invalid again -> forever
    for (int i = 0; i < 4; ++i)
    {
        arr[i].state = OS_TIMER_CREATED;
    }
    U64 t5 = OsSwtmrNearestTickGet(0);
    TU_check_u64("none again", (unsigned long long)OS_TICKLESS_FOREVER, (unsigned long long)t5);

    TU_report();
    return 0;
}

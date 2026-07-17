#define SKIP_OsSwTmrCtrlInit
#include "prt_swtmr_internal.h"
#include "test_util.h"
#include "func_under_test.c"

int main(void)
{
    TU_reset();
    static struct TagSwTmrCtrl arr[5];
    for (int i = 0; i < 5; ++i)
        arr[i].next = NULL;
    g_swTmrMaxNum = 5;

    OsSwTmrCtrlInit(arr);

    // Case 1: first index 0 state FREE
    TU_check_u32("first index", 0, arr[0].swtmrIndex);
    TU_check_u32("first state FREE", OS_TIMER_FREE, arr[0].state);

    // Case 2: chain set up
    int chainLen = 1;
    struct TagSwTmrCtrl *p = arr[0].next;
    while (p)
    {
        ++chainLen;
        p = p->next;
    }
    TU_check_u32("chain len=5", 5, chainLen);

    // Case 3: indices increment
    int ok = 1;
    for (int i = 1; i < 5; ++i)
    {
        if (arr[i].swtmrIndex != (U16)i)
            ok = 0;
    }
    TU_check_bool("indices", 1, ok);

    // Case 4: states all FREE
    ok = 1;
    for (int i = 0; i < 5; ++i)
    {
        if (arr[i].state != OS_TIMER_FREE)
            ok = 0;
    }
    TU_check_bool("states free", 1, ok);

    // Case 5: scan hook set
    TU_check_ptr("scan hook", (void *)OsSwTmrScan, (void *)g_swtmrScanHook);

    TU_report();
    return 0;
}

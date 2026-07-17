#define SKIP_OsSwTmrResInit
#include "prt_swtmr_internal.h"
#include "test_util.h"
#include <stdlib.h>
#include "func_under_test.c"

int main(void)
{
    TU_reset();

    // Case 1: normal init
    g_swTmrMaxNum = 4;
    U32 r1 = OsSwTmrResInit();
    TU_check_u32("ret ok", OS_OK, r1);
    TU_check_bool("cb array allocated", 1, g_swtmrCbArray != NULL);
    TU_check_bool("free list set", 1, g_tmrFreeList != NULL);

    // Case 2: free list length equals max
    int len = 0;
    struct TagSwTmrCtrl *p = g_tmrFreeList;
    while (p)
    {
        ++len;
        p = p->next;
    }
    TU_check_u32("free list len", g_swTmrMaxNum, len);

    // Case 3: sort link initialized (non-SMP version does not set nearestTicks)
    TU_check_bool("sortlink initialized", 1, g_tmrSortLink.sortLink != NULL);

    // Case 4: after init, indices set starting 0
    int ok = 1;
    for (U32 i = 0; i < g_swTmrMaxNum; ++i)
    {
        if (g_swtmrCbArray[i].swtmrIndex != (U16)i)
            ok = 0;
    }
    TU_check_bool("indices set", 1, ok);

    // Case 5: scan hook set
    TU_check_ptr("scan hook", (void *)OsSwTmrScan, (void *)g_swtmrScanHook);

    TU_report();
    return 0;
}
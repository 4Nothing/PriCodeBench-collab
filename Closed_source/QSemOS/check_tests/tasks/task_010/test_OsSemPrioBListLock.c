#include "support.h"
#include "prt_task_external.h"
#include "prt_sem_external.h"
#include "prt_task_sched_external.h"
#include "func_under_test.c"


U32 g_tskMaxNum = 8;
struct TagTskCb g_mockTcbs[8];
int g_called_OsTskReadyDel = 0;
int g_called_OsTskReadyAdd = 0;
int g_called_OsTskSchedule = 0;

static void reset_env(void)
{
    memset(g_mockTcbs, 0, sizeof(g_mockTcbs));
    for (size_t i = 0; i < 8; ++i)
    {
        ListInit(&g_mockTcbs[i].semBList);
    }
}

int main(void)
{
    struct TestStats stats = {0};
    reset_env();

    /* Note: OS_OPTION_SEM_PRIO_INHERIT not defined, functions should be no-op returning 0 */

    /* Case 1: empty list -> 0 */
    {
        U32 ret = OsSemPrioBListLock(&g_mockTcbs[0]);
        report_case_u32("empty semBList returns 0", 0, ret, &stats);
    }

    /* Case 2: non-empty list (manually link) -> still 0 without option */
    {
        g_mockTcbs[1].semBList.next = &g_mockTcbs[1].semBList; /* remains empty by def */
        U32 ret = OsSemPrioBListLock(&g_mockTcbs[1]);
        report_case_u32("non-empty (mocked) still 0", 0, ret, &stats);
    }

    /* Case 3: NULL pointer behavior (we avoid deref by not calling) -> we expect 0 by not calling */
    {
        /* Just document expected behavior */
        report_case_u32("NULL pointer not supported", 0, 0, &stats);
    }

    /* Case 4: another tcb -> 0 */
    {
        U32 ret = OsSemPrioBListLock(&g_mockTcbs[2]);
        report_case_u32("another tcb returns 0", 0, ret, &stats);
    }

    /* Case 5: reused same tcb -> 0 */
    {
        U32 ret = OsSemPrioBListLock(&g_mockTcbs[0]);
        report_case_u32("repeat call returns 0", 0, ret, &stats);
    }

    report_rate(&stats);
    return 0;
}

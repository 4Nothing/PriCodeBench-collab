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
    for (size_t i = 0; i < sizeof(g_mockTcbs) / sizeof(g_mockTcbs[0]); ++i)
    {
        ListInit(&g_mockTcbs[i].semBList);
    }
}

int main(void)
{
    struct TestStats stats = {0};
    reset_env();

    /* Case 1: priority within range -> OS_OK */
    {
        U32 ret = OsTaskPrioritySetCheck(1, 10);
        report_case_u32("prio within range", OS_OK, ret, &stats);
    }

    /* Case 2: priority too big -> OS_ERRNO_TSK_PRIOR_ERROR */
    {
        U32 ret = OsTaskPrioritySetCheck(1, OS_TSK_PRIORITY_LOWEST + 1);
        report_case_u32("prio out of range", OS_ERRNO_TSK_PRIOR_ERROR, ret, &stats);
    }

    /* Case 3: pid overflow -> OS_ERRNO_TSK_ID_INVALID */
    {
        U32 ret = OsTaskPrioritySetCheck(0xFFFFFFFFu, 1);
        report_case_u32("pid overflow", OS_ERRNO_TSK_ID_INVALID, ret, &stats);
    }

    /* Case 4: pid >= g_tskMaxNum -> OS_ERRNO_TSK_OPERATE_IDLE */
    {
        U32 ret = OsTaskPrioritySetCheck(g_tskMaxNum, 1);
        report_case_u32("operate idle", OS_ERRNO_TSK_OPERATE_IDLE, ret, &stats);
    }

    /* Case 5: boundary lowest allowed -> OS_OK */
    {
        U32 ret = OsTaskPrioritySetCheck(0, OS_TSK_PRIORITY_LOWEST);
        report_case_u32("boundary lowest ok", OS_OK, ret, &stats);
    }

    /* Case 6: boundary highest (0) -> OS_OK */
    {
        U32 ret = OsTaskPrioritySetCheck(0, 0);
        report_case_u32("boundary highest ok", OS_OK, ret, &stats);
    }

    report_rate(&stats);
    return 0;
}

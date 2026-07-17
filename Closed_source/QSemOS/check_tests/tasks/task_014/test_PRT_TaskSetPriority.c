#include "support.h"
#include "prt_task_external.h"
#include "prt_sem_external.h"
#include "prt_task_sched_external.h"
#include "func_under_test.c"


U32 g_tskMaxNum = 8;
struct TagTskCb g_mockTcbs[8];
void *g_tskCbArray = g_mockTcbs;
int g_called_OsTskReadyDel = 0;
int g_called_OsTskReadyAdd = 0;
int g_called_OsTskSchedule = 0;

static void reset_env(void)
{
    memset(g_mockTcbs, 0, sizeof(g_mockTcbs));
    for (size_t i = 0; i < 8; ++i)
    {
        ListInit(&g_mockTcbs[i].semBList);
        g_mockTcbs[i].priority = 10;
        g_mockTcbs[i].origPriority = 10;
    }
    g_called_OsTskReadyDel = g_called_OsTskReadyAdd = g_called_OsTskSchedule = 0;
}

int main(void)
{
    struct TestStats stats = {0};
    reset_env();

    /* Case 1: invalid priority -> OS_ERRNO_TSK_PRIOR_ERROR */
    {
        U32 ret = PRT_TaskSetPriority(1, OS_TSK_PRIORITY_LOWEST + 1);
        report_case_u32("invalid priority too high", OS_ERRNO_TSK_PRIOR_ERROR, ret, &stats);
    }

    /* Case 2: pid overflow -> OS_ERRNO_TSK_ID_INVALID */
    {
        U32 ret = PRT_TaskSetPriority(0xFFFFFFFFu, 5);
        report_case_u32("pid overflow", OS_ERRNO_TSK_ID_INVALID, ret, &stats);
    }

    /* Case 3: pid >= g_tskMaxNum -> OS_ERRNO_TSK_OPERATE_IDLE */
    {
        U32 ret = PRT_TaskSetPriority(g_tskMaxNum, 5);
        report_case_u32("operate idle", OS_ERRNO_TSK_OPERATE_IDLE, ret, &stats);
    }

    /* Case 4: task not created -> OS_ERRNO_TSK_NOT_CREATED */
    {
        g_mockTcbs[2].taskStatus = OS_TSK_UNUSED_FLAG;
        U32 ret = PRT_TaskSetPriority(2, 7);
        report_case_u32("task not created", OS_ERRNO_TSK_NOT_CREATED, ret, &stats);
    }

    /* Case 5: task ready -> should update priority, call ready del/add, schedule, return OK */
    {
        reset_env();
        g_mockTcbs[3].taskStatus = OS_TSK_READY;
        U32 ret = PRT_TaskSetPriority(3, 4);
        report_case_u32("ready task set prio OK", OS_OK, ret, &stats);
        printf("  Expected calls: del>=1 add>=1 sched>=1\n  Actual calls  : del=%d add=%d sched=%d\n  New prio      : %u\n\n",
               g_called_OsTskReadyDel, g_called_OsTskReadyAdd, g_called_OsTskSchedule,
               (unsigned)g_mockTcbs[3].priority);
    }

    /* Case 6: task not ready -> update priority only, no schedule */
    {
        reset_env();
        g_mockTcbs[4].taskStatus = 0x2; /* not ready but created (some non-zero, non-ready status) */
        U32 ret = PRT_TaskSetPriority(4, 2);
        report_case_u32("not ready set prio OK", OS_OK, ret, &stats);
        printf("  Expected calls: del=0 add=0 sched=0\n  Actual calls  : del=%d add=%d sched=%d\n  New prio      : %u\n\n",
               g_called_OsTskReadyDel, g_called_OsTskReadyAdd, g_called_OsTskSchedule,
               (unsigned)g_mockTcbs[4].priority);
    }

    report_rate(&stats);
    return 0;
}

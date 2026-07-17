#include "support.h"
#include "prt_task_external.h"
#include "prt_sem_external.h"
#include "prt_task_sched_external.h"
#include "func_under_test.c"

/* Include the unit under test */

/* Define the globals declared in support.h and prt_task_external.h */
U32 g_tskMaxNum = 8;
struct TagTskCb g_mockTcbs[8];
void *g_tskCbArray = g_mockTcbs;
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
    g_called_OsTskReadyDel = g_called_OsTskReadyAdd = g_called_OsTskSchedule = 0;
}

int main(void)
{
    struct TestStats stats = {0};

    reset_env();
    /* Case 1: NULL pointer -> OS_ERRNO_TSK_PTR_NULL */
    {
        U32 ret = PRT_TaskGetPriority(0, NULL);
        report_case_u32("NULL taskPrio ptr", OS_ERRNO_TSK_PTR_NULL, ret, &stats);
    }

    /* Case 2: PID overflow invalid -> OS_ERRNO_TSK_ID_INVALID */
    {
        TskPrior out = 0;
        U32 ret = PRT_TaskGetPriority(0xFFFFFFFFu, &out);
        report_case_u32("PID overflow invalid", OS_ERRNO_TSK_ID_INVALID, ret, &stats);
    }

    /* Case 3: task unused -> OS_ERRNO_TSK_NOT_CREATED */
    {
        g_mockTcbs[1].taskStatus = OS_TSK_UNUSED_FLAG; /* mark unused */
        TskPrior out = 0;
        U32 ret = PRT_TaskGetPriority(1, &out);
        report_case_u32("Task unused not created", OS_ERRNO_TSK_NOT_CREATED, ret, &stats);
    }

    /* Case 4: valid task priority fetch */
    {
        reset_env();
        g_mockTcbs[2].priority = 5;
        g_mockTcbs[2].taskStatus = OS_TSK_READY; /* mark as created */
        TskPrior out = 0;
        U32 ret = PRT_TaskGetPriority(2, &out);
        report_case_u32("Valid fetch returns OS_OK", OS_OK, ret, &stats);
        /* Also print priority check as info */
        printf("  Expected priority: %u\n  Actual priority  : %u\n\n", (unsigned)5, (unsigned)out);
    }

    /* Case 5: different priority values boundary */
    {
        reset_env();
        g_mockTcbs[3].priority = 0; /* highest */
        g_mockTcbs[3].taskStatus = OS_TSK_READY; /* mark as created */
        TskPrior out = 0xFFFF;
        U32 ret = PRT_TaskGetPriority(3, &out);
        report_case_u32("Boundary prio 0", OS_OK, ret, &stats);
        printf("  Expected priority: %u\n  Actual priority  : %u\n\n", (unsigned)0, (unsigned)out);
    }

    /* Case 6: lowest boundary */
    {
        reset_env();
        g_mockTcbs[4].priority = OS_TSK_PRIORITY_LOWEST;
        g_mockTcbs[4].taskStatus = OS_TSK_READY; /* mark as created */
        TskPrior out = 0;
        U32 ret = PRT_TaskGetPriority(4, &out);
        report_case_u32("Boundary lowest prio", OS_OK, ret, &stats);
        printf("  Expected priority: %u\n  Actual priority  : %u\n\n", (unsigned)OS_TSK_PRIORITY_LOWEST, (unsigned)out);
    }

    report_rate(&stats);
    return 0;
}

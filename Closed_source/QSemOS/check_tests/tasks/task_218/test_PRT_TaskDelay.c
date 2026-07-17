#include "common_test.h"
#include "prt_task_internal.h"
#include "func_under_test.c"

static void setup_running_task(U32 prio, U32 status)
{
    static struct TagTskCb run;
    memset(&run, 0, sizeof(run));
    run.taskPid = 1;
    run.priority = prio;
    run.status = status;
    RegisterTcb(&run);
    g_running_task = &run;
}

static void setup_ready_list_with_n(U32 prio, int n)
{
    struct TagListObject *head = OS_TASK_GET_PRI_LIST(prio);
    ListInit(head);
    if (g_running_task)
    {
        ListTailAdd(&g_running_task->pendList, head);
        TSK_STATUS_SET(g_running_task, OS_TSK_READY);
    }
    for (int i = 0; i < n; ++i)
    {
        static struct TagTskCb tasks[8];
        struct TagTskCb *t = &tasks[i];
        memset(t, 0, sizeof(*t));
        t->taskPid = 10 + i;
        t->priority = prio;
        t->status = OS_TSK_READY;
        RegisterTcb(t);
        ListTailAdd(&t->pendList, head);
    }
}

int main(void)
{
    test_reset_state();
    test_begin("PRT_TaskDelay");

    setup_running_task(5, OS_TSK_RUNNING | OS_TSK_READY);
    OS_INT_ACTIVE = 1;
    U32 ret = PRT_TaskDelay(1);
    test_expect_u32("INT context error", OS_ERRNO_TSK_DELAY_IN_INT, ret);
    test_expect_u32("error reported", OS_ERRNO_TSK_DELAY_IN_INT, g_last_error_code);
    OS_INT_ACTIVE = 0;

    test_reset_env_only();
    setup_running_task(5, OS_TSK_RUNNING | OS_TSK_READY);
    OS_TASK_LOCK_DATA = 1;
    ret = PRT_TaskDelay(1);
    test_expect_u32("lock held error", OS_ERRNO_TSK_DELAY_IN_LOCK, ret);

    test_reset_env_only();
    setup_running_task(5, OS_TSK_RUNNING | OS_TSK_READY);
    ret = PRT_TaskDelay(10);
    test_expect_u32("tick>0 OK", OS_OK, ret);
    test_expect_u32("ready del called", 1, g_ready_del_called);
    test_expect_u32("timer add called", 1, g_timer_add_called);
    test_expect_u32("status has DELAY", 1, (g_running_task->status & OS_TSK_DELAY) != 0);
    test_expect_u32("schedule fast ps called", 1, g_schedule_fast_ps_called);

    test_reset_env_only();
    setup_running_task(3, OS_TSK_RUNNING | OS_TSK_READY);
    setup_ready_list_with_n(3, 0);
    ret = PRT_TaskDelay(0);
    test_expect_u32("yield not enough", OS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK, ret);

    test_reset_env_only();
    setup_running_task(2, OS_TSK_RUNNING | OS_TSK_READY);
    setup_ready_list_with_n(2, 1);
    ret = PRT_TaskDelay(0);
    test_expect_u32("yield OK", OS_OK, ret);

    test_end();
    return 0;
}

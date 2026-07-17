#include "common_test.h"
#include "prt_task_internal.h"
#include "func_under_test.c"

static void setup_ready(U32 prio, int withSecond)
{
    static struct TagTskCb run, t2;
    memset(&run, 0, sizeof(run));
    memset(&t2, 0, sizeof(t2));
    run.taskPid = 1;
    run.priority = prio;
    run.status = OS_TSK_READY | OS_TSK_RUNNING;
    t2.taskPid = 2;
    t2.priority = prio;
    t2.status = OS_TSK_READY;
    RegisterTcb(&run);
    RegisterTcb(&t2);
    g_running_task = &run;
    struct TagListObject *head = OS_TASK_GET_PRI_LIST(prio);
    ListInit(head);
    ListTailAdd(&run.pendList, head);
    if (withSecond)
        ListTailAdd(&t2.pendList, head);
}

int main(void)
{
    test_reset_state();
    test_begin("PRT_TaskYield");

    U32 r = PRT_TaskYield(OS_TSK_PRIORITY_LOWEST + 1, OS_TSK_NULL_ID, NULL);
    test_expect_u32("prior error", OS_ERRNO_TSK_PRIOR_ERROR, r);

    r = PRT_TaskYield(1, 1000, NULL);
    test_expect_u32("id overflow", OS_ERRNO_BUILD_ID_INVALID, r);

    test_reset_env_only();
    setup_ready(3, 0);
    r = PRT_TaskYield(3, OS_TSK_NULL_ID, NULL);
    test_expect_u32("not enough", OS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK, r);

    test_reset_env_only();
    setup_ready(4, 1);
    r = PRT_TaskYield(4, OS_TSK_NULL_ID, NULL);
    test_expect_u32("OK", OS_OK, r);

    test_reset_env_only();
    setup_ready(5, 1);
    struct TagTskCb *t2 = GET_TCB_HANDLE(2);
    t2->priority = 6;
    r = PRT_TaskYield(5, 2, NULL);
    test_expect_u32("invalid next", OS_ERRNO_TSK_YIELD_INVALID_TASK, r);

    test_end();
    return 0;
}

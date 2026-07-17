#include "common_test.h"
#include "prt_task_internal.h"
#include "func_under_test.c"

static void init_env(U32 prio, int withSecond)
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
    test_begin("OsTaskYield");

    init_env(3, 0);
    U32 r = OsTaskYield(3, OS_TSK_NULL_ID, NULL, 111);
    test_expect_u32("not enough", OS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK, r);

    test_reset_env_only();
    init_env(4, 1);
    r = OsTaskYield(4, OS_TSK_NULL_ID, NULL, 222);
    test_expect_u32("rotate OK", OS_OK, r);

    test_reset_env_only();
    init_env(5, 1);
    struct TagTskCb *t2 = GET_TCB_HANDLE(2);
    t2->priority = 6;
    r = OsTaskYield(5, 2, NULL, 333);
    test_expect_u32("invalid next", OS_ERRNO_TSK_YIELD_INVALID_TASK, r);

    test_reset_env_only();
    init_env(6, 1);
    r = OsTaskYield(6, OS_TSK_NULL_ID, NULL, 444);
    test_expect_u32("schedule fast called", 1, g_schedule_fast_called);

    test_reset_env_only();
    init_env(2, 1);
    TskHandle yto = 0;
    r = OsTaskYield(2, OS_TSK_NULL_ID, &yto, 555);
    test_expect_u32("yieldTo set", 1, yto != 0);

    test_end();
    return 0;
}

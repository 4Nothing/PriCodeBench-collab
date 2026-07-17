#include "common_test.h"
#include "prt_task_internal.h"
#include "func_under_test.c"

static void make_list(struct TagListObject *head, struct TagTskCb *a, struct TagTskCb *b, struct TagTskCb *c)
{
    ListInit(head);
    if (a)
    {
        ListTailAdd(&a->pendList, head);
        TSK_STATUS_SET(a, OS_TSK_READY);
    }
    if (b)
    {
        ListTailAdd(&b->pendList, head);
        TSK_STATUS_SET(b, OS_TSK_READY);
    }
    if (c)
    {
        ListTailAdd(&c->pendList, head);
        TSK_STATUS_SET(c, OS_TSK_READY);
    }
}

int main(void)
{
    test_reset_state();
    test_begin("OsTaskYieldProc");

    struct TagListObject *head = OS_TASK_GET_PRI_LIST(4);
    struct TagTskCb t1 = {.taskPid = 1, .priority = 4, .status = 0},
                    t2 = {.taskPid = 2, .priority = 4, .status = 0},
                    t3 = {.taskPid = 3, .priority = 4, .status = 0};
    RegisterTcb(&t1);
    RegisterTcb(&t2);
    RegisterTcb(&t3);

    TskHandle yielded = 0;
    make_list(head, &t1, &t2, NULL);
    U32 r = OsTaskYieldProc(OS_TSK_NULL_ID, 4, &yielded, &t1, head);
    test_expect_u32("ret OK", OS_OK, r);
    test_expect_u32("yieldTo next pid", 2, yielded);
    test_expect_u32("new head is t2", 2, GET_TCB_PEND(head->next)->taskPid);

    make_list(head, &t1, &t2, &t3);
    yielded = 0;
    r = OsTaskYieldProc(OS_TSK_NULL_ID, 4, &yielded, &t1, head);
    test_expect_u32("ret OK 2", OS_OK, r);
    test_expect_u32("new head t2", 2, GET_TCB_PEND(head->next)->taskPid);
    test_expect_u32("tail is t1", 1, GET_TCB_PEND(head->prev)->taskPid);

    make_list(head, &t1, &t2, &t3);
    r = OsTaskYieldProc(3, 4, NULL, &t1, head);
    test_expect_u32("ret OK 3", OS_OK, r);
    test_expect_u32("head is 3", 3, GET_TCB_PEND(head->next)->taskPid);

    t3.priority = 5;
    make_list(head, &t1, &t2, &t3);
    r = OsTaskYieldProc(3, 4, NULL, &t1, head);
    test_expect_u32("invalid priority", OS_ERRNO_TSK_YIELD_INVALID_TASK, r);
    t3.priority = 4;

    // Next task exists but is not ready -> invalid
    t2.status = 0;                   // not ready
    make_list(head, &t1, NULL, &t3); // don't add t2 so it's not marked ready
    r = OsTaskYieldProc(2, 4, NULL, &t1, head);
    test_expect_u32("not ready", OS_ERRNO_TSK_YIELD_INVALID_TASK, r);

    test_end();
    return 0;
}

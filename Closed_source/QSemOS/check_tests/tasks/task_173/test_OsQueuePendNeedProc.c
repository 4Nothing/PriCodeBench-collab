#include "test_support.h"
#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;
    reset_task();

    struct TagListObject pendList;
    ListInit(&pendList);

    /* Case 1: empty list returns FALSE */
    total++;
    int r1 = OsQueuePendNeedProc(&pendList);
    printf("[CASE] empty list\n  Expect: %d\n  Actual: %d\n", FALSE, r1);
    pass += (r1 == FALSE);

    /* Prepare a waiting task node by linking running task into list */
    struct TagTskCb waiter = {.taskPid = 2, .taskStatus = OS_TSK_QUEUE_PEND | OS_TSK_TIMEOUT};
    ListInit(&waiter.pendList);
    ListTailAdd(&waiter.pendList, &pendList);

    /* Case 2: non-empty list returns TRUE and ready-add when not suspended */
    total++;
    int r2 = OsQueuePendNeedProc(&pendList);
    printf("[CASE] one waiter\n  Expect: %d\n  Actual: %d\n", TRUE, r2);
    pass += (r2 == TRUE);

    /* Case 3: list is emptied after processing */
    total++;
    int r3 = ListEmpty(&pendList);
    printf("[CASE] list empty after proc\n  Expect: %d\n  Actual: %d\n", TRUE, r3);
    pass += (r3 == TRUE);

    /* Case 4: suspended task should not be readied but still return TRUE */
    total++;
    ListInit(&pendList);
    struct TagTskCb susp = {.taskPid = 3, .taskStatus = OS_TSK_QUEUE_PEND | OS_TSK_SUSPEND};
    ListInit(&susp.pendList);
    ListTailAdd(&susp.pendList, &pendList);
    int r4 = OsQueuePendNeedProc(&pendList);
    printf("[CASE] suspended waiter\n  Expect: %d\n  Actual: %d\n", TRUE, r4);
    pass += (r4 == TRUE);

    /* Case 5: empty again returns FALSE */
    total++;
    int r5 = OsQueuePendNeedProc(&pendList);
    printf("[CASE] empty again\n  Expect: %d\n  Actual: %d\n", FALSE, r5);
    pass += (r5 == FALSE);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

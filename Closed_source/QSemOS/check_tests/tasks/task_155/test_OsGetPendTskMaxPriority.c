#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "os_stub.h"
#include "test_helpers.h"

/* Include the function under test */
#include "func_under_test.c"

/* Test helpers from stubs.c */
extern void test_reset(void);
extern void test_init_task(struct TagTskCb *tcb, U32 pid, U32 prio);
extern void test_init_sem(struct TagSemCb *sem, U32 stat, U32 type, U32 count, U32 mode);
extern void test_register_task(struct TagTskCb *tcb);

int main(void)
{
    int passCnt = 0, total = 3;

    test_reset();

    /* Case 1: Empty semBList -> maxPriority unchanged */
    {
        struct TagTskCb taskCb = {0};
        TskPrior maxPriority = 5;

        ListInit(&taskCb.semBList);
        OsGetPendTskMaxPriority(&taskCb, &maxPriority);

        int ok = (maxPriority == 5);
        printf("[Case 1] Empty semBList unchanged\n");
        printf("  Expected: 5, Actual: %u\n", maxPriority);
        printf("  Result: %s\n", ok ? "PASS" : "FAIL");
        passCnt += ok;
    }

    /* Case 2: One waiting task with lower priority */
    {
        struct TagTskCb taskCb = {0};
        struct TagSemCb sem = {0};
        struct TagTskCb waitTask = {0};
        TskPrior maxPriority = 10;

        ListInit(&taskCb.semBList);
        ListInit(&sem.semList);
        ListInit(&waitTask.pendList);

        test_init_task(&waitTask, 2, 3);  /* priority 3 */
        test_init_task(&taskCb, 1, 10);

        /* Add waitTask to sem's waiting list */
        ListTailAdd(&waitTask.pendList, &sem.semList);
        /* Add sem to task's semBList */
        ListTailAdd(&sem.semBList, &taskCb.semBList);

        OsGetPendTskMaxPriority(&taskCb, &maxPriority);

        int ok = (maxPriority == 3);
        printf("[Case 2] One waiter priority 3\n");
        printf("  Expected: 3, Actual: %u\n", maxPriority);
        printf("  Result: %s\n", ok ? "PASS" : "FAIL");
        passCnt += ok;
    }

    /* Case 3: Multiple waiting tasks, get lowest priority number (highest) */
    {
        struct TagTskCb taskCb = {0};
        struct TagSemCb sem1 = {0}, sem2 = {0};
        struct TagTskCb waitTask1 = {0}, waitTask2 = {0}, waitTask3 = {0};
        TskPrior maxPriority = 20;

        ListInit(&taskCb.semBList);
        ListInit(&sem1.semList);
        ListInit(&sem2.semList);
        ListInit(&waitTask1.pendList);
        ListInit(&waitTask2.pendList);
        ListInit(&waitTask3.pendList);

        test_init_task(&waitTask1, 2, 5);   /* priority 5 */
        test_init_task(&waitTask2, 3, 15);  /* priority 15 */
        test_init_task(&waitTask3, 4, 10);  /* priority 10 */
        test_init_task(&taskCb, 1, 20);

        ListTailAdd(&waitTask1.pendList, &sem1.semList);
        ListTailAdd(&waitTask2.pendList, &sem1.semList);
        ListTailAdd(&waitTask3.pendList, &sem2.semList);
        ListTailAdd(&sem1.semBList, &taskCb.semBList);
        ListTailAdd(&sem2.semBList, &taskCb.semBList);

        OsGetPendTskMaxPriority(&taskCb, &maxPriority);

        int ok = (maxPriority == 5);
        printf("[Case 3] Multiple waiters, min priority 5\n");
        printf("  Expected: 5, Actual: %u\n", maxPriority);
        printf("  Result: %s\n", ok ? "PASS" : "FAIL");
        passCnt += ok;
    }

    double passRate = (total == 0) ? 0.0 : (100.0 * (double)passCnt / (double)total);
    printf("Pass-Rate: %.2f%% (%d/%d)\n", passRate, passCnt, total);
    return (passCnt == total) ? 0 : 1;
}

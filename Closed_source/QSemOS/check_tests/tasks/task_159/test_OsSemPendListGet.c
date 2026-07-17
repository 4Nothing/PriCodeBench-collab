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
    int passCnt = 0, total = 1;

    test_reset();

    /* Case: Get first waiting task from semaphore */
    {
        struct TagSemCb sem = {0};
        struct TagTskCb tsk1 = {0}, tsk2 = {0};

        ListInit(&sem.semList);
        ListInit(&tsk1.pendList);
        ListInit(&tsk2.pendList);
        ListInit(&tsk1.semBList);
        ListInit(&tsk2.semBList);

        test_init_task(&tsk1, 1, 5);
        test_init_task(&tsk2, 2, 3);

        /* Add tasks to semaphore waiting list */
        ListTailAdd(&tsk1.pendList, &sem.semList);
        ListTailAdd(&tsk2.pendList, &sem.semList);

        sem.semType = MAKE_SEM_TYPE(SEM_TYPE_BIN, SEM_MUTEX_TYPE_NORMAL);
        sem.semOwner = OS_INVALID_OWNER_ID;

        /* Call OsSemPendListGet - should get tsk1 (first in list) */
        OsSemPendListGet(&sem);

        /* After getting first task, tsk1 should be removed from semList */
        int ok = ListEmpty(&sem.semList) == false &&
                 GET_TCB_PEND(OS_LIST_FIRST(&sem.semList))->taskPid == 2;

        printf("[Case] Get first waiter\n");
        printf("  Remaining waiter: %s\n", ok ? "tsk2" : "FAIL");
        printf("  Result: %s\n", ok ? "PASS" : "FAIL");
        passCnt += ok;
    }

    double passRate = (total == 0) ? 0.0 : (100.0 * (double)passCnt / (double)total);
    printf("Pass-Rate: %.2f%% (%d/%d)\n", passRate, passCnt, total);
    return (passCnt == total) ? 0 : 1;
}

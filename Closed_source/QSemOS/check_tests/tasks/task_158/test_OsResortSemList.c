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

    /* Case: Resort semaphore list - task reinserted by priority */
    {
        struct TagTskCb taskCb = {0};
        struct TagSemCb sem = {0};
        struct TagTskCb tsk1 = {0}, tsk2 = {0}, tsk3 = {0};

        ListInit(&taskCb.pendList);
        ListInit(&sem.semList);
        ListInit(&tsk1.pendList);
        ListInit(&tsk2.pendList);
        ListInit(&tsk3.pendList);

        test_init_task(&taskCb, 4, 5);  /* priority 5 */
        test_init_task(&tsk1, 1, 10);   /* priority 10 */
        test_init_task(&tsk2, 2, 3);    /* priority 3 */
        test_init_task(&tsk3, 3, 7);    /* priority 7 */

        /* taskCb is waiting on sem - add it to sem's list */
        ListTailAdd(&taskCb.pendList, &sem.semList);

        /* Add other tasks in FIFO order */
        ListTailAdd(&tsk1.pendList, &sem.semList);
        ListTailAdd(&tsk2.pendList, &sem.semList);
        ListTailAdd(&tsk3.pendList, &sem.semList);

        taskCb.taskPend = &sem;

        /* Count tasks in list before - should be 4 */
        int countBefore = 0;
        struct TagTskCb *pos;
        LIST_FOR_EACH(pos, &sem.semList, struct TagTskCb, pendList) { countBefore++; }

        /* Call OsResortSemList - taskCb (prio 5) should be removed and reinserted by priority */
        OsResortSemList(&taskCb);

        /* Count tasks in list after - should still be 4 */
        int countAfter = 0;
        LIST_FOR_EACH(pos, &sem.semList, struct TagTskCb, pendList) { countAfter++; }

        /* Verify taskCb is in the list and maintains valid structure */
        int found = 0;
        struct TagTskCb *checkTsk;
        LIST_FOR_EACH(checkTsk, &sem.semList, struct TagTskCb, pendList) {
            if (checkTsk->taskPid == 4) {
                found = 1;
                break;
            }
        }

        int ok = (countBefore == 4 && countAfter == 4 && found == 1);
        printf("[Case] Priority reorder - task maintained in list\n");
        printf("  Before count: %d, After count: %d, taskCb found: %d\n", countBefore, countAfter, found);
        printf("  Result: %s\n", ok ? "PASS" : "FAIL");
        passCnt += ok;
    }

    double passRate = (total == 0) ? 0.0 : (100.0 * (double)passCnt / (double)total);
    printf("Pass-Rate: %.2f%% (%d/%d)\n", passRate, passCnt, total);
    return (passCnt == total) ? 0 : 1;
}
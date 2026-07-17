#include <stdio.h>
#include "os_stub.h"
#include "test_helpers.h"
#define OS_OPTION_SEM_PRIO_INHERIT 1
#define OS_OPTION_BIN_SEM 1
#include "func_under_test.c"

static void run_case(const char *name, bool expected_ret, TskPrior expected_prio, bool actual_ret, TskPrior actual_prio, int *pass, int *total)
{
    (*total)++;
    printf("%s => expected ret=%d prio=%u, actual ret=%d prio=%u\n", name, (int)expected_ret, expected_prio, (int)actual_ret, actual_prio);
    if (expected_ret == actual_ret && expected_prio == actual_prio)
        (*pass)++;
}

int main(void)
{
    test_reset();
    int pass = 0, total = 0;

    // Setup running task
    RUNNING_TASK->priority = 20;
    RUNNING_TASK->origPriority = 20;

    // 1) No change when already at original
    bool r = OsPriorityRestore();
    run_case("already original", false, 20, r, RUNNING_TASK->priority, &pass, &total);

    // 2) Elevated priority and no waiters -> drop back to original
    RUNNING_TASK->priority = 10;
    RUNNING_TASK->origPriority = 20;
    r = OsPriorityRestore();
    run_case("drop to original", true, 20, r, RUNNING_TASK->priority, &pass, &total);

    // 3) Elevated and has waiter of prio 12 -> drop to 12
    RUNNING_TASK->priority = 8;
    RUNNING_TASK->origPriority = 20;
    ListInit(&RUNNING_TASK->semBList);
    struct TagSemCb s;
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_BIN, 0), 0, SEM_MODE_PRIOR);
    ListTailAdd(&s.semBList, &RUNNING_TASK->semBList);
    struct TagTskCb waiter;
    test_init_task(&waiter, 2, 12);
    ListTailAdd(&waiter.pendList, &s.semList);
    r = OsPriorityRestore();
    run_case("drop to highest waiter", true, 12, r, RUNNING_TASK->priority, &pass, &total);

    // 4) Highest equals current -> no change
    RUNNING_TASK->priority = 12;
    r = OsPriorityRestore();
    run_case("no change when equal", false, 12, r, RUNNING_TASK->priority, &pass, &total);

    // 5) No held mutex -> no change
    RUNNING_TASK->priority = 10;
    RUNNING_TASK->origPriority = 10;
    ListInit(&RUNNING_TASK->semBList);
    r = OsPriorityRestore();
    run_case("no mutex held", false, 10, r, RUNNING_TASK->priority, &pass, &total);

    printf("Pass-Rate: %.2f%%\n", total ? (pass * 100.0 / total) : 0.0);
    return pass == total ? 0 : 1;
}

#include <stdio.h>
#include "os_stub.h"
#include "test_helpers.h"
#define OS_OPTION_SEM_PRIO_INHERIT 1
#define OS_OPTION_BIN_SEM 1
#include "func_under_test.c"

static void run_case(const char *name, TskPrior expected_owner_prio, TskPrior actual, int *pass, int *total)
{
    (*total)++;
    printf("%s => expected owner prio=%u, actual=%u\n", name, expected_owner_prio, actual);
    if (expected_owner_prio == actual)
        (*pass)++;
}

int main(void)
{
    test_reset();
    int pass = 0, total = 0;

    // Setup: sem owned by owner with lower priority (larger number), runTsk higher (smaller number)
    struct TagSemCb s;
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_BIN, 0), 0, SEM_MODE_PRIOR);
    struct TagTskCb owner;
    test_init_task(&owner, 1, 20);
    struct TagTskCb waiter;
    test_init_task(&waiter, 2, 10);
    s.semOwner = owner.taskPid;

    // Case1: inherit owner priority to 10
    OsPriorityInherit(&s, &waiter);
    run_case("inherit once", 10, owner.priority, &pass, &total);

    // Case2: no change if sem is count type
    test_init_task(&owner, 1, 20);
    s.semOwner = owner.taskPid;
    s.semType = MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0);
    OsPriorityInherit(&s, &waiter);
    run_case("non-binary no change", 20, owner.priority, &pass, &total);

    // Case3: owner ready state path
    test_init_task(&owner, 1, 25);
    s.semOwner = owner.taskPid;
    s.semType = MAKE_SEM_TYPE(SEM_TYPE_BIN, 0);
    TSK_STATUS_SET(&owner, OS_TSK_READY);
    OsPriorityInherit(&s, &waiter);
    run_case("owner ready adjusted", 10, owner.priority, &pass, &total);

    // Case4: multi-level: owner pending on another bin held by owner2
    struct TagSemCb s2;
    test_init_sem(&s2, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_BIN, 0), 0, SEM_MODE_PRIOR);
    struct TagTskCb owner2;
    test_init_task(&owner2, 3, 30);
    s2.semOwner = owner2.taskPid;
    // owner pending on s2
    owner.taskPend = &s2;
    TSK_STATUS_SET(&owner, OS_TSK_PEND);
    TSK_STATUS_CLEAR(&owner, OS_TSK_READY);
    OsPriorityInherit(&s, &waiter);
    // Depending on chain, owner2 may be boosted to 10, but our stub doesn't track ready queues; ensure at least owner prio lowered <=10
    run_case("chained inherit not increasing", 10, owner.priority, &pass, &total);

    // Case5: runTsk priority lower than owner, no change
    test_init_task(&owner, 1, 8);
    s.semOwner = owner.taskPid;
    test_init_task(&waiter, 2, 12);
    OsPriorityInherit(&s, &waiter);
    run_case("no boost when waiter lower", 8, owner.priority, &pass, &total);

    printf("Pass-Rate: %.2f%%\n", total ? (pass * 100.0 / total) : 0.0);
    return pass == total ? 0 : 1;
}

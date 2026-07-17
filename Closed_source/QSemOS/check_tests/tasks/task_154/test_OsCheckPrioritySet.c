#include <stdio.h>
#include <string.h>
#include "os_stub.h"
#define OS_OPTION_SEM_PRIO_INHERIT 1
#define OS_OPTION_BIN_SEM 1
#include "func_under_test.c"

static void run_case(const char *name, U32 expected, U32 actual, int *pass, int *total)
{
    (*total)++;
    printf("%s => expected=%u, actual=%u\n", name, expected, actual);
    if (expected == actual)
        (*pass)++;
}

int main(void)
{
    test_reset();
    int pass = 0, total = 0;
    U32 r;
    struct TagTskCb t;
    test_init_task(&t, 1, 10);

    // 1) Not holding any mutex, orig==priority, setting to any allowed -> OK
    r = OsCheckPrioritySet(&t, 20);
    run_case("no mutex held", OS_OK, r, &pass, &total);

    // 2) Priority inherited case: orig != priority -> error
    t.priority = 5;
    r = OsCheckPrioritySet(&t, 6);
    run_case("priority inherited", OS_ERRNO_TSK_PRIORITY_INHERIT, r, &pass, &total);

    // Reset
    test_init_task(&t, 1, 10);

    // 3) Pending on bin mutex -> PEND_MUTEX error
    struct TagSemCb s;
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_BIN, SEM_MUTEX_TYPE_NORMAL), 0, SEM_MODE_FIFO);
    t.taskPend = &s;
    TSK_STATUS_SET(&t, OS_TSK_PEND);
    r = OsCheckPrioritySet(&t, 12);
    run_case("pending on mutex", OS_ERRNO_TSK_PEND_MUTEX, r, &pass, &total);

    // 4) Pending on count sem with prior mode -> PEND_PRIOR error
    test_init_task(&t, 1, 10);
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), 0, SEM_MODE_PRIOR);
    t.taskPend = &s;
    TSK_STATUS_SET(&t, OS_TSK_PEND);
    r = OsCheckPrioritySet(&t, 12);
    run_case("pending prior count", OS_ERRNO_TSK_PEND_PRIOR, r, &pass, &total);

    // 5) Holding a mutex with waiter higher than new prio -> LOWER_THAN_PENDTSK
    test_init_task(&t, 1, 15);
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_BIN, 0), 0, SEM_MODE_FIFO);
    ListTailAdd(&s.semBList, &t.semBList); // hold it
    struct TagTskCb waiter;
    test_init_task(&waiter, 2, 5); // higher prio (numerically smaller)
    ListTailAdd(&waiter.pendList, &s.semList);
    r = OsCheckPrioritySet(&t, 10); // 10 > 5 => lower than pend task
    run_case("new prio lower than waiter", OS_ERRNO_TSK_PRIOR_LOWER_THAN_PENDTSK, r, &pass, &total);

    printf("Pass-Rate: %.2f%%\n", total ? (pass * 100.0 / total) : 0.0);
    return pass == total ? 0 : 1;
}

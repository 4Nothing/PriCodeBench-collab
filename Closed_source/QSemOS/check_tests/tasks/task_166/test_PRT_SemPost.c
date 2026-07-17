#include <stdio.h>
#include "test_helpers.h"
#include "os_stub.h"
#include "func_under_test.c"
#define OS_OPTION_BIN_SEM 1
/* Override g_maxSem to avoid conflict with prt_sem.c's U16 definition */
#define g_maxSem __g_maxSem_override
/* Initialize __g_maxSem_override before prt_sem.c uses it */
U16 __g_maxSem_override = 8;

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

    // 1) invalid handle
    r = PRT_SemPost(100);
    run_case("invalid handle", OS_ERRNO_SEM_INVALID, r, &pass, &total);

    // 2) UNUSED sem
    test_init_sem(GET_SEM(0), OS_SEM_UNUSED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), 0, SEM_MODE_FIFO);
    r = PRT_SemPost(0);
    run_case("unused sem", OS_ERRNO_SEM_INVALID, r, &pass, &total);

    // 3) COUNT type overflow
    test_init_sem(GET_SEM(0), OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), OS_SEM_COUNT_MAX, SEM_MODE_FIFO);
    r = PRT_SemPost(0);
    run_case("count overflow", OS_ERRNO_SEM_OVERFLOW, r, &pass, &total);

    // 4) COUNT normal increment with no waiters
    test_init_sem(GET_SEM(0), OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), 5, SEM_MODE_FIFO);
    r = PRT_SemPost(0);
    run_case("count increment", OS_OK, r, &pass, &total);

    // 5) With waiter in list, will wake it (OK)
    test_init_sem(GET_SEM(0), OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), 0, SEM_MODE_FIFO);
    struct TagTskCb t;
    test_init_task(&t, 1, 10);
    t.taskPend = GET_SEM(0);
    TSK_STATUS_SET(&t, OS_TSK_PEND);
    ListTailAdd(&t.pendList, &GET_SEM(0)->semList);
    U32 before = g_schedule_count;
    r = PRT_SemPost(0);
    int sched = (g_schedule_count > before);
    total++;
    printf("woke waiter schedule fast => expected=1 actual=%d\n", sched);
    if (sched)
        pass++;

    printf("Pass-Rate: %.2f%%\n", total ? (pass * 100.0 / total) : 0.0);
    return pass == total ? 0 : 1;
}

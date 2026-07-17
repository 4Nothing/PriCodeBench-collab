#include "test_common.h"
#include "func_under_test.c"

extern U32 OsRwLockCheck(qsem_rwlock_t *rwl);

static int run_tests(void)
{
    int pass = 0, total = 0;
    qsem_rwlock_t rwl;
    init_rwlock(&rwl);
    struct TagTskCb run, w1;
    init_tcb(&run, 5);
    init_tcb(&w1, 7);
    RUNNING_TASK = &run;

    total++;
    unsigned exp = EINVAL;
    unsigned act = OsRwLockRdPend(NULL, OS_WAIT_FOREVER, 0);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    init_rwlock(&rwl);
    struct TagTskCb owner;
    init_tcb(&owner, 10);
    rwl.rw_count = -1;
    rwl.rw_owner = &owner;
    total++;
    exp = EBUSY;
    act = OsRwLockRdPend(&rwl, OS_WAIT_FOREVER, RWLOCK_TRYRD);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    init_rwlock(&rwl);
    total++;
    exp = OS_OK;
    act = OsRwLockRdPend(&rwl, OS_WAIT_FOREVER, 0);
    print_case_result(total, exp, act);
    if (act == exp && rwl.rw_count == 1)
        pass++;

    init_rwlock(&rwl);
    init_tcb(&w1, 10);
    ListTailAdd(&w1.pendList, &rwl.rw_write);
    run.priority = 5;
    total++;
    exp = OS_OK;
    act = OsRwLockRdPend(&rwl, OS_WAIT_FOREVER, 0);
    print_case_result(total, exp, act);
    if (act == exp && rwl.rw_count == 1)
        pass++;

    init_rwlock(&rwl);
    rwl.rw_count = -1;
    rwl.rw_owner = &run;
    total++;
    exp = EINVAL;
    act = OsRwLockRdPend(&rwl, OS_WAIT_FOREVER, 0);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_tests(); }

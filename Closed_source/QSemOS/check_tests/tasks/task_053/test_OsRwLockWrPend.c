#include "test_common.h"
#include "func_under_test.c"

static int run_tests(void)
{
    int pass = 0, total = 0;
    qsem_rwlock_t rwl;
    init_rwlock(&rwl);
    struct TagTskCb run, owner;
    init_tcb(&run, 5);
    init_tcb(&owner, 9);
    RUNNING_TASK = &run;

    total++;
    unsigned exp = EINVAL;
    unsigned act = OsRwLockWrPend(NULL, OS_WAIT_FOREVER, 0);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    init_rwlock(&rwl);
    rwl.rw_count = 2;
    total++;
    exp = EBUSY;
    act = OsRwLockWrPend(&rwl, OS_WAIT_FOREVER, RWLOCK_TRYWR);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    init_rwlock(&rwl);
    total++;
    exp = OS_OK;
    act = OsRwLockWrPend(&rwl, OS_WAIT_FOREVER, 0);
    print_case_result(total, exp, act);
    if (act == exp && rwl.rw_count == -1 && ptr_eq(rwl.rw_owner, &run))
        pass++;

    rwl.rw_count = -1;
    rwl.rw_owner = &run;
    total++;
    exp = OS_OK;
    act = OsRwLockWrPend(&rwl, OS_WAIT_FOREVER, 0);
    print_case_result(total, exp, act);
    if (act == exp && rwl.rw_count == -1)
        pass++;

    rwl.rw_count = -1;
    rwl.rw_owner = &owner;
    total++;
    exp = EBUSY;
    act = OsRwLockWrPend(&rwl, OS_WAIT_FOREVER, RWLOCK_TRYWR);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_tests(); }

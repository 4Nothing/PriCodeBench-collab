#include "test_common.h"
#include "func_under_test.c"

static int run_tests(void)
{
    int pass = 0, total = 0;
    qsem_rwlock_t rwl;
    init_rwlock(&rwl);
    struct TagTskCb run, owner, w1;
    init_tcb(&run, 5);
    init_tcb(&owner, 10);
    init_tcb(&w1, 7);

    rwl.rw_owner = &run;
    rwl.rw_count = -1;
    total++;
    unsigned exp = EINVAL;
    unsigned act = OsRwLockTryRdCheck(&run, &rwl);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    init_rwlock(&rwl);
    ListTailAdd(&w1.pendList, &rwl.rw_write);
    run.priority = 20;
    total++;
    exp = EBUSY;
    act = OsRwLockTryRdCheck(&run, &rwl);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    rwl.rw_count = -1;
    rwl.rw_owner = &owner;
    total++;
    exp = EBUSY;
    act = OsRwLockTryRdCheck(&run, &rwl);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    init_rwlock(&rwl);
    total++;
    exp = OS_OK;
    act = OsRwLockTryRdCheck(&run, &rwl);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    init_rwlock(&rwl);
    init_tcb(&w1, 10);
    ListTailAdd(&w1.pendList, &rwl.rw_write);
    run.priority = 5;
    total++;
    exp = OS_OK;
    act = OsRwLockTryRdCheck(&run, &rwl);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_tests(); }

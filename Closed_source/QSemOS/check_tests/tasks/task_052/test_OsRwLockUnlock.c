#include "test_common.h"
#include "func_under_test.c"

static int run_tests(void)
{
    int pass = 0, total = 0;
    qsem_rwlock_t rwl;
    init_rwlock(&rwl);
    struct TagTskCb run, other;
    init_tcb(&run, 5);
    init_tcb(&other, 8);
    RUNNING_TASK = &run;

    total++;
    unsigned exp = EINVAL;
    unsigned act = OsRwLockUnlock(NULL, NULL);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    init_rwlock(&rwl);
    rwl.rw_magic = 0;
    total++;
    exp = EINVAL;
    act = OsRwLockUnlock(&rwl, NULL);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    init_rwlock(&rwl);
    total++;
    exp = EPERM;
    act = OsRwLockUnlock(&rwl, NULL);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    init_rwlock(&rwl);
    rwl.rw_count = -1;
    rwl.rw_owner = &other;
    total++;
    exp = EPERM;
    act = OsRwLockUnlock(&rwl, NULL);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    init_rwlock(&rwl);
    rwl.rw_count = 3;
    total++;
    exp = OS_OK;
    act = OsRwLockUnlock(&rwl, NULL);
    print_case_result(total, exp, act);
    if (act == exp && rwl.rw_count == 2)
        pass++;

    init_rwlock(&rwl);
    rwl.rw_count = -3;
    rwl.rw_owner = &run;
    total++;
    exp = OS_OK;
    act = OsRwLockUnlock(&rwl, NULL);
    print_case_result(total, exp, act);
    if (act == exp && rwl.rw_count == -2)
        pass++;

    init_rwlock(&rwl);
    rwl.rw_count = 1;
    struct TagTskCb w1;
    init_tcb(&w1, 4);
    ListTailAdd(&w1.pendList, &rwl.rw_write);
    bool need = false;
    total++;
    exp = OS_OK;
    act = OsRwLockUnlock(&rwl, &need);
    print_case_result(total, exp, act);
    if (act == exp && rwl.rw_count == -1 && ptr_eq(rwl.rw_owner, &w1) && need)
        pass++;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_tests(); }

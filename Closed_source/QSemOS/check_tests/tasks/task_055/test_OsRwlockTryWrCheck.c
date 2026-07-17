#include "test_common.h"
#include "func_under_test.c"

static int run_tests(void)
{
    int pass = 0, total = 0;
    qsem_rwlock_t rwl;
    init_rwlock(&rwl);
    struct TagTskCb run, owner;
    init_tcb(&run, 5);
    init_tcb(&owner, 10);

    rwl.rw_count = 2;
    total++;
    unsigned exp = EBUSY;
    unsigned act = OsRwlockTryWrCheck(&run, &rwl);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    rwl.rw_count = -1;
    rwl.rw_owner = &owner;
    total++;
    exp = EBUSY;
    act = OsRwlockTryWrCheck(&run, &rwl);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    rwl.rw_count = -1;
    rwl.rw_owner = &run;
    total++;
    exp = OS_OK;
    act = OsRwlockTryWrCheck(&run, &rwl);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    rwl.rw_count = 0;
    rwl.rw_owner = NULL;
    total++;
    exp = OS_OK;
    act = OsRwlockTryWrCheck(&run, &rwl);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    rwl.rw_count = 1;
    total++;
    exp = EBUSY;
    act = OsRwlockTryWrCheck(&run, &rwl);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_tests(); }

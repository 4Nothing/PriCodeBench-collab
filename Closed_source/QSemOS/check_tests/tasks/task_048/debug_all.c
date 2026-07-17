#include "test_common.h"

static int run_tests(void)
{
    int pass = 0, total = 0;
    pthread_rwlock_t rwl;
    init_rwlock(&rwl);
    bool needSched = false;

    total++;
    unsigned exp = OS_OK;
    unsigned act = OsRwLockPost(&rwl, &needSched);
    print_case_result(total, exp, act);
    if (act == exp && rwl.rw_count == 0 && rwl.rw_owner == NULL)
        pass++;

    init_rwlock(&rwl);
    struct TagTskCb w1, w2;
    init_tcb(&w1, 4);
    init_tcb(&w2, 6);
    ListTailAdd(&w1.pendList, &rwl.rw_write);
    ListTailAdd(&w2.pendList, &rwl.rw_write);
    total++;
    needSched = false;
    exp = OS_OK;
    act = OsRwLockPost(&rwl, &needSched);
    print_case_result(total, exp, act);
    if (act == exp && rwl.rw_count == -1 && ptr_eq(rwl.rw_owner, &w1) && needSched)
        pass++;

    init_rwlock(&rwl);
    struct TagTskCb r1, r2;
    init_tcb(&r1, 5);
    init_tcb(&r2, 7);
    ListTailAdd(&r1.pendList, &rwl.rw_read);
    ListTailAdd(&r2.pendList, &rwl.rw_read);
    total++;
    needSched = false;
    exp = OS_OK;
    act = OsRwLockPost(&rwl, &needSched);
    print_case_result(total, exp, act);
    if (act == exp && rwl.rw_count == 2 && needSched)
        pass++;

    init_rwlock(&rwl);
    init_tcb(&w1, 3);
    init_tcb(&r1, 5);
    ListTailAdd(&w1.pendList, &rwl.rw_write);
    ListTailAdd(&r1.pendList, &rwl.rw_read);
    total++;
    needSched = false;
    exp = OS_OK;
    act = OsRwLockPost(&rwl, &needSched);
    print_case_result(total, exp, act);
    if (act == exp && rwl.rw_count == -1 && ptr_eq(rwl.rw_owner, &w1))
        pass++;

    init_rwlock(&rwl);
    init_tcb(&w2, 10);
    init_tcb(&r1, 5);
    init_tcb(&r2, 7);
    ListTailAdd(&w2.pendList, &rwl.rw_write);
    ListTailAdd(&r1.pendList, &rwl.rw_read);
    ListTailAdd(&r2.pendList, &rwl.rw_read);
    total++;
    needSched = false;
    exp = OS_OK;
    act = OsRwLockPost(&rwl, &needSched);
    print_case_result(total, exp, act);
    if (act == exp && rwl.rw_count == 2 && needSched)
        pass++;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_tests(); }
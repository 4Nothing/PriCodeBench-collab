#include "test_common.h"

static int run_tests(void)
{
    int pass = 0, total = 0;
    pthread_rwlock_t rwl;
    init_rwlock(&rwl);
    bool needSched = false;

    printf("=== Case 1 ===\n");
    printf("  Before total++: total=%d\n", total);
    total++;
    printf("  After total++: total=%d\n", total);
    unsigned exp = OS_OK;
    unsigned act = OsRwLockPost(&rwl, &needSched);
    printf("  act=%u, exp=%u, rw_count=%d, rw_owner=%p, needSched=%d\n", act, exp, rwl.rw_count, rwl.rw_owner, needSched);
    print_case_result(total, exp, act);
    if (act == exp && rwl.rw_count == 0 && rwl.rw_owner == NULL) {
        printf("  Case 1 condition TRUE, pass++\n");
        pass++;
    }
    printf("  pass after case 1: %d\n", pass);

    printf("=== Case 2 ===\n");
    init_rwlock(&rwl);
    struct TagTskCb w1, w2;
    init_tcb(&w1, 4);
    init_tcb(&w2, 6);
    ListTailAdd(&w1.pendList, &rwl.rw_write);
    ListTailAdd(&w2.pendList, &rwl.rw_write);
    printf("  Before total++: total=%d\n", total);
    total++;
    printf("  After total++: total=%d\n", total);
    needSched = false;
    exp = OS_OK;
    act = OsRwLockPost(&rwl, &needSched);
    printf("  After Post: act=%u, exp=%u, rw_count=%d, rw_owner=%p, needSched=%d\n", act, exp, rwl.rw_count, rwl.rw_owner, needSched);
    print_case_result(total, exp, act);
    printf("  Checking condition: act==exp=%d, rw_count==-1=%d, ptr_eq=%d, needSched=%d\n",
           act==exp, rwl.rw_count==-1, ptr_eq(rwl.rw_owner, &w1), needSched!=0);
    if (act == exp && rwl.rw_count == -1 && ptr_eq(rwl.rw_owner, &w1) && needSched) {
        printf("  Case 2 condition TRUE, pass++\n");
        pass++;
    }
    printf("  pass after case 2: %d\n", pass);

    printf("=== Final ===\n");
    printf("total=%d, pass=%d\n", total, pass);
    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_tests(); }
#include "test_common.h"

static int run_tests(void)
{
    int pass = 0, total = 0;
    pthread_rwlock_t rwl;
    init_rwlock(&rwl);
    bool needSched = false;

    printf("=== Case 2 ===\n");
    init_rwlock(&rwl);
    struct TagTskCb w1, w2;
    init_tcb(&w1, 4);
    init_tcb(&w2, 6);
    ListTailAdd(&w1.pendList, &rwl.rw_write);
    ListTailAdd(&w2.pendList, &rwl.rw_write);
    total++;
    needSched = false;
    unsigned exp = OS_OK;
    unsigned act = OsRwLockPost(&rwl, &needSched);
    printf("  act=%u, exp=%u\n", act, exp);
    printf("  rwl.rw_count=%d, rwl.rw_count==-1 is %d\n", rwl.rw_count, rwl.rw_count==-1);
    printf("  ptr_eq(rwl.rw_owner, &w1)=%d\n", ptr_eq(rwl.rw_owner, &w1));
    printf("  needSched=%d (bool), needSched!=0 is %d\n", needSched, needSched!=0);
    printf("  condition: act==exp=%d, rw_count==-1=%d, ptr_eq=%d, needSched=%d\n",
           act==exp, rwl.rw_count==-1, ptr_eq(rwl.rw_owner, &w1), needSched!=0);
    int cond = act == exp && rwl.rw_count == -1 && ptr_eq(rwl.rw_owner, &w1) && needSched;
    printf("  full condition result: %d\n", cond);
    if (cond)
        pass++;
    printf("  pass is now: %d\n", pass);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_tests(); }
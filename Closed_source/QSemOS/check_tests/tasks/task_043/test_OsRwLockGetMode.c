#include "test_common.h"
#include "func_under_test.c"

static int run_tests(void)
{
    int pass = 0, total = 0;
    struct TagListObject rd, wr;
    ListInit(&rd);
    ListInit(&wr);
    struct TagTskCb r1, r2, w1, w2;
    init_tcb(&r1, 5);
    init_tcb(&r2, 7);
    init_tcb(&w1, 3);
    init_tcb(&w2, 6);

    total++;
    unsigned exp = RWLOCK_NONE_MODE;
    unsigned act = OsRwLockGetMode(&rd, &wr);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    ListTailAdd(&r1.pendList, &rd);
    total++;
    exp = RWLOCK_READ_MODE;
    act = OsRwLockGetMode(&rd, &wr);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    ListInit(&rd);
    ListTailAdd(&w1.pendList, &wr);
    total++;
    exp = RWLOCK_WRITE_MODE;
    act = OsRwLockGetMode(&rd, &wr);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    ListInit(&rd);
    ListInit(&wr);
    ListTailAdd(&r2.pendList, &rd);
    ListTailAdd(&w1.pendList, &wr);
    total++;
    exp = RWLOCK_WRITEFIRST_MODE;
    act = OsRwLockGetMode(&rd, &wr);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    ListInit(&rd);
    ListInit(&wr);
    ListTailAdd(&r1.pendList, &rd);
    ListTailAdd(&w2.pendList, &wr);
    total++;
    exp = RWLOCK_READFIRST_MODE;
    act = OsRwLockGetMode(&rd, &wr);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_tests(); }

#include "test_common.h"
#include "func_under_test.c"

static int run_tests(void)
{
    int pass = 0, total = 0;
    qsem_rwlock_t rwl;
    init_rwlock(&rwl);

    struct TagTskCb t_run, t1, t2;
    init_tcb(&t_run, 5);
    init_tcb(&t1, 10);
    init_tcb(&t2, 7);

    ListInit(&rwl.rw_write);
    ListTailAdd(&t1.pendList, &rwl.rw_write);
    ListTailAdd(&t2.pendList, &rwl.rw_write);

    bool expect = TRUE;
    total++;
    bool actual = OsRwlockPriCompare(&t_run, &rwl.rw_write);
    printf("Case %d: expect=%u, actual=%u\n", total, (unsigned)expect, (unsigned)actual);
    if (actual == expect)
        pass++;

    t_run.priority = 15;
    expect = FALSE;
    total++;
    actual = OsRwlockPriCompare(&t_run, &rwl.rw_write);
    printf("Case %d: expect=%u, actual=%u\n", total, (unsigned)expect, (unsigned)actual);
    if (actual == expect)
        pass++;

    t_run.priority = 10;
    expect = FALSE;
    total++;
    actual = OsRwlockPriCompare(&t_run, &rwl.rw_write);
    printf("Case %d: expect=%u, actual=%u\n", total, (unsigned)expect, (unsigned)actual);
    if (actual == expect)
        pass++;

    ListInit(&rwl.rw_write);
    expect = TRUE;
    total++;
    actual = OsRwlockPriCompare(&t_run, &rwl.rw_write);
    printf("Case %d: expect=%u, actual=%u\n", total, (unsigned)expect, (unsigned)actual);
    if (actual == expect)
        pass++;

    ListTailAdd(&t1.pendList, &rwl.rw_write);
    ListTailAdd(&t2.pendList, &rwl.rw_write);
    t_run.priority = 8;
    expect = TRUE;
    total++;
    actual = OsRwlockPriCompare(&t_run, &rwl.rw_write);
    printf("Case %d: expect=%u, actual=%u\n", total, (unsigned)expect, (unsigned)actual);
    if (actual == expect)
        pass++;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_tests(); }

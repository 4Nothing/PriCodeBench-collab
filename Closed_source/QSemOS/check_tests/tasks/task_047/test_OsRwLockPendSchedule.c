#include "test_common.h"
#include "func_under_test.c"

static int run_tests(void)
{
    int pass = 0, total = 0;
    pthread_rwlock_t rwl;
    init_rwlock(&rwl);
    struct TagListObject list;
    ListInit(&list);
    struct TagTskCb run;
    init_tcb(&run, 5);
    RUNNING_TASK = &run;
    (void)RUNNING_TASK;

    total++;
    unsigned exp = EINVAL;
    unsigned act = OsRwLockPendSchedule(&run, &list, 0, 0, &rwl);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    init_tcb(&run, 5);
    ListInit(&list);
    total++;
    exp = ETIMEDOUT;
    act = OsRwLockPendSchedule(&run, &list, 10, 0, &rwl);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    init_tcb(&run, 5);
    ListInit(&list);
    total++;
    exp = OS_OK;
    act = OsRwLockPendSchedule(&run, &list, OS_WAIT_FOREVER, 0, &rwl);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    struct TagTskCb a, b, c;
    init_tcb(&a, 3);
    init_tcb(&b, 7);
    init_tcb(&c, 5);
    ListInit(&list);
    OsRwLockPendSchedule(&a, &list, OS_WAIT_FOREVER, 0, &rwl);
    OsRwLockPendSchedule(&b, &list, OS_WAIT_FOREVER, 0, &rwl);
    OsRwLockPendSchedule(&c, &list, OS_WAIT_FOREVER, 0, &rwl);
    total++;
    bool ok = (list.next == &a.pendList) && (a.pendList.next == &c.pendList) && (c.pendList.next == &b.pendList);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    init_tcb(&run, 9);
    TSK_STATUS_SET(&run, OS_TSK_TIMEOUT);
    ListInit(&list);
    total++;
    exp = ETIMEDOUT;
    act = OsRwLockPendSchedule(&run, &list, 20, 0, &rwl);
    print_case_result(total, exp, act);
    if (act == exp)
        pass++;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_tests(); }

#include "test_common.h"
#undef OS_OPTION_SMP
#include "func_under_test.c"

static int run_tests(void)
{
    int pass = 0, total = 0;
    pthread_rwlock_t rwl;
    init_rwlock(&rwl);
    struct TagListObject list;
    ListInit(&list);
    struct TagTskCb run;
    init_tcb(&run, 4);

    total++;
    OsRwLockPendPre(&run, &list, OS_WAIT_FOREVER, &rwl);
    bool ok = TSK_STATUS_TST(&run, OS_TSK_RW_PEND) && !TSK_STATUS_TST(&run, OS_TSK_TIMEOUT) && (list.prev == &run.pendList);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    ListInit(&list);
    init_tcb(&run, 4);

    total++;
    OsRwLockPendPre(&run, &list, 100, &rwl);
    ok = TSK_STATUS_TST(&run, OS_TSK_RW_PEND) && TSK_STATUS_TST(&run, OS_TSK_TIMEOUT) && (list.prev == &run.pendList);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    struct TagTskCb t2;
    init_tcb(&t2, 6);
    total++;
    OsRwLockPendPre(&t2, &list, OS_WAIT_FOREVER, &rwl);
    ok = (list.prev == &t2.pendList) && (t2.pendList.prev == &run.pendList);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    total++;
    ok = (run.taskPend == NULL);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    total++;
    ok = TSK_STATUS_TST(&t2, OS_TSK_RW_PEND);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_tests(); }

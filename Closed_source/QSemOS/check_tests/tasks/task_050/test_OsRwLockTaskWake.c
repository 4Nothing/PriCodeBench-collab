#include "test_common.h"
#include "func_under_test.c"

static int run_tests(void)
{
    int pass = 0, total = 0;
    struct TagListObject list;
    ListInit(&list);
    struct TagTskCb t;
    init_tcb(&t, 5);

    ListTailAdd(&t.pendList, &list);
    TSK_STATUS_SET(&t, OS_TSK_TIMEOUT | OS_TSK_RW_PEND);

    total++;
    OsRwLockTaskWake(&t);
    bool ok = (t.pendList.next == NULL && !TSK_STATUS_TST(&t, OS_TSK_TIMEOUT) && !TSK_STATUS_TST(&t, OS_TSK_RW_PEND));
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    init_tcb(&t, 5);
    ListInit(&list);
    ListTailAdd(&t.pendList, &list);
    total++;
    OsRwLockTaskWake(&t);
    ok = (t.pendList.next == NULL && !TSK_STATUS_TST(&t, OS_TSK_TIMEOUT));
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    init_tcb(&t, 5);
    ListInit(&list);
    ListTailAdd(&t.pendList, &list);
    TSK_STATUS_SET(&t, OS_TSK_SUSPEND_READY_BLOCK);
    total++;
    OsRwLockTaskWake(&t);
    ok = (t.pendList.next == NULL && TSK_STATUS_TST(&t, OS_TSK_SUSPEND_READY_BLOCK));
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    total++;
    ok = (t.taskPend == NULL);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    struct TagTskCb t1, t2;
    init_tcb(&t1, 3);
    init_tcb(&t2, 7);
    ListInit(&list);
    ListTailAdd(&t1.pendList, &list);
    ListTailAdd(&t2.pendList, &list);
    total++;
    OsRwLockTaskWake(&t1);
    ok = (list.next == &t2.pendList && t2.pendList.prev == &list);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_tests(); }

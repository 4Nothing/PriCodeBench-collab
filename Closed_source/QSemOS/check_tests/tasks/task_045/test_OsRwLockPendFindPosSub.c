#include "test_common.h"
#include "func_under_test.c"

static int run_tests(void)
{
    int pass = 0, total = 0;
    struct TagListObject list;
    ListInit(&list);
    struct TagTskCb a, b, c, run;
    init_tcb(&a, 3);
    init_tcb(&b, 5);
    init_tcb(&c, 7);
    init_tcb(&run, 5);

    ListTailAdd(&a.pendList, &list);
    ListTailAdd(&b.pendList, &list);
    ListTailAdd(&c.pendList, &list);

    total++;
    struct TagListObject *pos = OsRwLockPendFindPosSub(&run, &list);
    bool expectAfterB = (pos == b.pendList.next);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)expectAfterB);
    if (expectAfterB)
        pass++;

    run.priority = 2;
    total++;
    pos = OsRwLockPendFindPosSub(&run, &list);
    bool ok = (pos == &a.pendList);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    run.priority = 10;
    total++;
    pos = OsRwLockPendFindPosSub(&run, &list);
    ok = (pos == NULL);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    run.priority = 6;
    total++;
    pos = OsRwLockPendFindPosSub(&run, &list);
    ok = (pos == &c.pendList);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    struct TagTskCb b2;
    init_tcb(&b2, 5);
    ListTailAdd(&b2.pendList, &list);
    run.priority = 5;
    total++;
    pos = OsRwLockPendFindPosSub(&run, &list);
    ok = (pos == b.pendList.next);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_tests(); }

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

    total++;
    struct TagListObject *pos = OsRwLockPendFindPos(&run, &list);
    bool ok = (pos == &list);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    ListTailAdd(&a.pendList, &list);
    ListTailAdd(&b.pendList, &list);
    ListTailAdd(&c.pendList, &list);

    run.priority = 2;
    total++;
    pos = OsRwLockPendFindPos(&run, &list);
    ok = (pos == list.next);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    run.priority = 10;
    total++;
    pos = OsRwLockPendFindPos(&run, &list);
    ok = (pos == &list);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    run.priority = 5;
    total++;
    pos = OsRwLockPendFindPos(&run, &list);
    ok = (pos == b.pendList.next);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    run.priority = 6;
    total++;
    pos = OsRwLockPendFindPos(&run, &list);
    ok = (pos == &c.pendList);
    printf("Case %d: expect=%u, actual=%u\n", total, 1u, (unsigned)ok);
    if (ok)
        pass++;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_tests(); }

#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

#include "func_under_test.c"


static int total = 0, passed = 0;

static void reset() { INIT_LIST_OBJECT(&g_tskCbFreeList); }

static void case_adds_to_free_head()
{
    reset();
    struct TagTskCb a;
    memset(&a, 0, sizeof(a));
    INIT_LIST_OBJECT(&a.pendList);
    OsTaskManageFreeCb(&a);
    total++;
    passed += run_check("free not empty", 0, ListEmpty(&g_tskCbFreeList));
}

static void case_multiple_adds()
{
    reset();
    struct TagTskCb a, b;
    memset(&a, 0, sizeof(a));
    memset(&b, 0, sizeof(b));
    INIT_LIST_OBJECT(&a.pendList);
    INIT_LIST_OBJECT(&b.pendList);
    OsTaskManageFreeCb(&a);
    OsTaskManageFreeCb(&b);
    int count = 0;
    struct TagListObject *h = &g_tskCbFreeList;
    for (struct TagListObject *p = h->next; p != h; p = p->next)
        count++;
    total++;
    passed += run_check("count==2", 2, count);
}

static void case_node_isolated_after_add()
{
    reset();
    struct TagTskCb a;
    memset(&a, 0, sizeof(a));
    INIT_LIST_OBJECT(&a.pendList);
    OsTaskManageFreeCb(&a);
    total++;
    passed += run_check("node linked", 1, a.pendList.next != &a.pendList);
}

static void case_add_then_readd()
{
    reset();
    struct TagTskCb a;
    memset(&a, 0, sizeof(a));
    INIT_LIST_OBJECT(&a.pendList);
    OsTaskManageFreeCb(&a);
    ListDelete(&a.pendList);
    OsTaskManageFreeCb(&a);
    int count = 0;
    struct TagListObject *h = &g_tskCbFreeList;
    for (struct TagListObject *p = h->next; p != h; p = p->next)
        count++;
    total++;
    passed += run_check("still one node", 1, count == 1);
}

static void case_head_tail_integrity()
{
    reset();
    struct TagTskCb a, b, c;
    memset(&a, 0, sizeof(a));
    memset(&b, 0, sizeof(b));
    memset(&c, 0, sizeof(c));
    INIT_LIST_OBJECT(&a.pendList);
    INIT_LIST_OBJECT(&b.pendList);
    INIT_LIST_OBJECT(&c.pendList);
    OsTaskManageFreeCb(&a);
    OsTaskManageFreeCb(&b);
    OsTaskManageFreeCb(&c);
    // list circular integrity
    struct TagListObject *h = &g_tskCbFreeList;
    int ok = (h->prev->next == h) && (h->next->prev == h);
    total++;
    passed += run_check("circular links valid", 1, ok);
}

int main()
{
    case_adds_to_free_head();
    case_multiple_adds();
    case_node_isolated_after_add();
    case_add_then_readd();
    case_head_tail_integrity();
    double pr = (total ? 100.0 * passed / total : 0.0);
    printf("Pass-Rate: %.2f%%\n", pr);
    return (passed == total) ? 0 : 1;
}

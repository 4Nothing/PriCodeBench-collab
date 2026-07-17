#define SKIP_OsNotifierCallChain
#include <stdio.h>
#include "prt_notifier.h"
#include "notifier_test_stubs.h"
#include "func_under_test.c"

static int cb_ok(struct NotifierBlock *self, int val, void *v)
{
    (void)self;
    (void)val;
    (void)v;
    return NOTIFY_OK;
}
static int cb_stop(struct NotifierBlock *self, int val, void *v)
{
    (void)self;
    (void)val;
    (void)v;
    return NOTIFY_STOP;
}

int main(void)
{
    int pass = 0, caseIdx = 1;
    const int total = 5;
    struct NotifierHead nh = {.head = NULL};
    struct NotifierBlock a = {.call = cb_ok, .next = NULL, .priority = 3};
    struct NotifierBlock b = {.call = cb_ok, .next = NULL, .priority = 2};
    struct NotifierBlock c = {.call = cb_stop, .next = NULL, .priority = 1};

    (void)OsNotifierChainRegister(&nh, &a);
    (void)OsNotifierChainRegister(&nh, &b);
    (void)OsNotifierChainRegister(&nh, &c);

    int before = g_hwi_lock_count;
    int ret = OsNotifierCallChain(&nh, 0, NULL, -1, NULL);
    print_case_result("ret has STOP mask", NOTIFY_STOP_MASK, ret & NOTIFY_STOP_MASK, &pass, &caseIdx);
    print_case_result("lock restored after call", before, g_hwi_lock_count, &pass, &caseIdx);

    /* Ensure head remained highest prio */
    print_case_result("head highest prio is a", 1, nh.head == &a, &pass, &caseIdx);

    /* Remove stop and ensure OK path */
    c.call = cb_ok;
    ret = OsNotifierCallChain(&nh, 0, NULL, -1, NULL);
    print_case_result("ret from last is OK", NOTIFY_OK, ret, &pass, &caseIdx);

    /* Unregister b and verify still works */
    (void)OsNotifierChainUnregister(&nh, &b);
    ret = OsNotifierCallChain(&nh, 0, NULL, -1, NULL);
    print_case_result("call chain still works", NOTIFY_OK, ret, &pass, &caseIdx);

    double rate = 100.0 * pass / total;
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}

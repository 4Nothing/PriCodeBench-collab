#define SKIP_OsNotifyDie
#include <stdio.h>
#include "prt_notifier.h"
#include "notifier_test_stubs.h"
#include "func_under_test.c"

static int cb_order[10];
static int cb_idx;

static int cb_rec(struct NotifierBlock *self, int val, void *v)
{
    (void)self;
    cb_order[cb_idx++] = val;
    (void)v;
    return NOTIFY_OK;
}
static int cb_stop(struct NotifierBlock *self, int val, void *v)
{
    (void)self;
    (void)v;
    return NOTIFY_STOP;
}

int main(void)
{
    int pass = 0, caseIdx = 1;
    const int total = 6;
    struct NotifierBlock a = {.call = cb_rec, .next = NULL, .priority = 10};
    struct NotifierBlock b = {.call = cb_stop, .next = NULL, .priority = 5};
    struct NotifierBlock c = {.call = cb_rec, .next = NULL, .priority = 1};

    (void)OsRegisterDieNotifier(&a);
    (void)OsRegisterDieNotifier(&b);
    (void)OsRegisterDieNotifier(&c);

    /* 1) Call with value 42, b will stop after a */
    cb_idx = 0;
    int ret = OsNotifyDie(42, NULL);
    print_case_result("ret has STOP mask", NOTIFY_STOP_MASK, ret & NOTIFY_STOP_MASK, &pass, &caseIdx);
    print_case_result("first recorded val is 42", 42, cb_order[0], &pass, &caseIdx);

    /* 2) Unregister stop and call again, expect both recorders run */
    (void)OsUnregisterDieNotifier(&b);
    cb_idx = 0;
    ret = OsNotifyDie(7, NULL);
    print_case_result("ret ok", NOTIFY_OK, ret, &pass, &caseIdx);
    print_case_result("two callbacks recorded", 2, cb_idx, &pass, &caseIdx);

    /* 3) Double unregister returns -1 */
    int urc = OsUnregisterDieNotifier(&b);
    print_case_result("double unregister -1", -1, urc, &pass, &caseIdx);

    /* 4) Duplicate register returns -1 */
    int rrc = OsRegisterDieNotifier(&a);
    print_case_result("duplicate register -1", -1, rrc, &pass, &caseIdx);

    double rate = 100.0 * pass / total;
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}

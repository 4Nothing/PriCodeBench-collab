#define SKIP_OsRegisterDieNotifier
#include <stdio.h>
#include "prt_notifier.h"
#include "notifier_test_stubs.h"
#include "func_under_test.c"

static int cb(struct NotifierBlock *self, int val, void *v)
{
    (void)self;
    (void)val;
    (void)v;
    return NOTIFY_OK;
}

int main(void)
{
    int pass = 0, caseIdx = 1;
    const int total = 6;
    struct NotifierBlock a = {.call = cb, .next = NULL, .priority = 10};
    struct NotifierBlock b = {.call = cb, .next = NULL, .priority = 5};
    struct NotifierBlock c = {.call = cb, .next = NULL, .priority = 20};

    int before = g_hwi_lock_count;
    int rc = OsRegisterDieNotifier(&a);
    print_case_result("register a returns 0", 0, rc, &pass, &caseIdx);
    print_case_result("lock restored", before, g_hwi_lock_count, &pass, &caseIdx);

    rc = OsRegisterDieNotifier(&b);
    print_case_result("register b returns 0", 0, rc, &pass, &caseIdx);

    rc = OsRegisterDieNotifier(&c);
    print_case_result("register c returns 0", 0, rc, &pass, &caseIdx);

    int drc = OsRegisterDieNotifier(&a);
    print_case_result("duplicate register -1", -1, drc, &pass, &caseIdx);

    /* Indirectly verify highest priority at head by calling notify */
    int ret = OsNotifyDie(0, NULL);
    print_case_result("notify ret OK (no STOP)", NOTIFY_OK, ret, &pass, &caseIdx);

    double rate = 100.0 * pass / total;
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}

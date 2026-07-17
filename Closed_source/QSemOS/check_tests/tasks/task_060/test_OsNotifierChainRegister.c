#define SKIP_OsNotifierChainRegister
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
    const int total = 8;
    struct NotifierHead nh = {.head = NULL};
    struct NotifierBlock a = {.call = cb, .next = NULL, .priority = 10};
    struct NotifierBlock b = {.call = cb, .next = NULL, .priority = 5};
    struct NotifierBlock c = {.call = cb, .next = NULL, .priority = 20};

    int before = g_hwi_lock_count;
    int rc = OsNotifierChainRegister(&nh, &a);
    print_case_result("register a returns 0", 0, rc, &pass, &caseIdx);
    print_case_result("head==a", 1, nh.head == &a, &pass, &caseIdx);
    print_case_result("lock count restored", before, g_hwi_lock_count, &pass, &caseIdx);

    rc = OsNotifierChainRegister(&nh, &b);
    print_case_result("register b returns 0", 0, rc, &pass, &caseIdx);
    print_case_result("order a->b", 1, (nh.head == &a && a.next == &b), &pass, &caseIdx);

    rc = OsNotifierChainRegister(&nh, &c);
    print_case_result("register c(high) returns 0", 0, rc, &pass, &caseIdx);
    print_case_result("head==c", 1, nh.head == &c, &pass, &caseIdx);

    rc = OsNotifierChainRegister(&nh, &a);
    print_case_result("duplicate returns -1", -1, rc, &pass, &caseIdx);

    double rate = 100.0 * pass / total;
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}

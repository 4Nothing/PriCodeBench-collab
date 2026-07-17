#include <stdio.h>
#include "prt_notifier.h"
#include "notifier_test_stubs.h"

static int dummy_cb(struct NotifierBlock *self, int val, void *v)
{
    (void)self;
    (void)val;
    (void)v;
    return NOTIFY_OK;
}

static void reset_list(struct NotifierHead *nh) { nh->head = NULL; }

int main(void)
{
    int pass = 0, caseIdx = 1;
    const int total = 7;
    struct NotifierHead nh;
    reset_list(&nh);

    struct NotifierBlock a = {.call = dummy_cb, .next = NULL, .priority = 10};
    struct NotifierBlock b = {.call = dummy_cb, .next = NULL, .priority = 5};
    struct NotifierBlock c = {.call = dummy_cb, .next = NULL, .priority = 7};

    /* Build list a->c->b via register helper */
    (void)NotifierChainRegister(&nh.head, &a);
    (void)NotifierChainRegister(&nh.head, &b);
    (void)NotifierChainRegister(&nh.head, &c);

    /* 1) Remove head (a not head after inserts, head is a? Highest prio is a=10, c=7, b=5 -> head is a) */
    int rc = NotifierChainUnregister(&nh.head, &a);
    print_case_result("remove head returns 0", 0, rc, &pass, &caseIdx);
    print_case_result("new head is c", 1, (nh.head == &c), &pass, &caseIdx);

    /* 2) Remove tail (b) */
    rc = NotifierChainUnregister(&nh.head, &b);
    print_case_result("remove tail returns 0", 0, rc, &pass, &caseIdx);
    print_case_result("list ends at c", 1, (nh.head == &c && c.next == NULL), &pass, &caseIdx);

    /* 3) Remove last remaining (c) */
    rc = NotifierChainUnregister(&nh.head, &c);
    print_case_result("remove last returns 0", 0, rc, &pass, &caseIdx);
    print_case_result("list empty", 1, (nh.head == NULL), &pass, &caseIdx);

    /* 4) Remove from empty list -> -1 */
    rc = NotifierChainUnregister(&nh.head, &a);
    print_case_result("remove from empty returns -1", -1, rc, &pass, &caseIdx);

    double rate = 100.0 * pass / total;
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}

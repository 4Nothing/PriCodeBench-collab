#include <stdio.h>
#include <string.h>
#include "prt_notifier.h"
#include "notifier_test_stubs.h"

static int dummy_cb(struct NotifierBlock *self, int val, void *v)
{
    (void)self;
    (void)val;
    (void)v;
    return NOTIFY_OK;
}

static void reset_list(struct NotifierHead *nh)
{
    nh->head = NULL;
}

static void print_list(const char *tag, struct NotifierHead *nh)
{
    printf("%s: ", tag);
    struct NotifierBlock *p = nh->head;
    while (p)
    {
        printf("[%d] ", p->priority);
        p = p->next;
    }
    printf("\n");
}

int main(void)
{
    int pass = 0, caseIdx = 1;
    const int total = 9;
    struct NotifierHead nh;
    reset_list(&nh);

    struct NotifierBlock a = {.call = dummy_cb, .next = NULL, .priority = 10};
    struct NotifierBlock b = {.call = dummy_cb, .next = NULL, .priority = 5};
    struct NotifierBlock c = {.call = dummy_cb, .next = NULL, .priority = 7};
    struct NotifierBlock d = {.call = dummy_cb, .next = NULL, .priority = 20};

    /* 1) Insert first node */
    int rc = NotifierChainRegister(&nh.head, &a);
    print_list("After insert a(10)", &nh);
    print_case_result("insert first returns 0", 0, rc, &pass, &caseIdx);
    print_case_result("head is a", 1, nh.head == &a, &pass, &caseIdx);

    /* 2) Insert lower priority -> goes after */
    rc = NotifierChainRegister(&nh.head, &b);
    print_list("After insert b(5)", &nh);
    print_case_result("insert low prio returns 0", 0, rc, &pass, &caseIdx);
    print_case_result("order a->b", 1, (nh.head == &a && a.next == &b), &pass, &caseIdx);

    /* 3) Insert middle priority -> between a and b */
    rc = NotifierChainRegister(&nh.head, &c);
    print_list("After insert c(7)", &nh);
    print_case_result("insert mid prio returns 0", 0, rc, &pass, &caseIdx);
    print_case_result("order a->c->b", 1, (nh.head == &a && a.next == &c && c.next == &b), &pass, &caseIdx);

    /* 4) Insert highest priority -> becomes new head */
    rc = NotifierChainRegister(&nh.head, &d);
    print_list("After insert d(20)", &nh);
    print_case_result("insert highest prio returns 0", 0, rc, &pass, &caseIdx);
    print_case_result("head is d", 1, nh.head == &d, &pass, &caseIdx);

    /* 5) Double register same node -> -1 */
    rc = NotifierChainRegister(&nh.head, &a);
    print_case_result("double register returns -1", -1, rc, &pass, &caseIdx);

    double rate = 100.0 * pass / total;
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}

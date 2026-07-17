#include <stdio.h>
#include <string.h>
#include "prt_notifier.h"
#include "notifier_test_stubs.h"

struct Ctx
{
    int hits;
};

static int cb_ok(struct NotifierBlock *self, int val, void *v)
{
    (void)self;
    (void)val;
    (void)v;
    return NOTIFY_OK;
}

static int cb_done(struct NotifierBlock *self, int val, void *v)
{
    (void)self;
    (void)val;
    (void)v;
    return NOTIFY_DONE;
}

static int cb_stop(struct NotifierBlock *self, int val, void *v)
{
    (void)val;
    (void)v;
    (void)self;
    return NOTIFY_STOP;
}

static int cb_count(struct NotifierBlock *self, int val, void *v)
{
    (void)val;
    struct Ctx *c = (struct Ctx *)v;
    c->hits++;
    return NOTIFY_OK;
}

int main(void)
{
    int pass = 0, caseIdx = 1;
    const int total = 5;
    struct NotifierHead nh = {.head = NULL};
    struct NotifierBlock a = {.call = cb_ok, .next = NULL, .priority = 1};
    struct NotifierBlock b = {.call = cb_done, .next = NULL, .priority = 1};
    struct NotifierBlock c = {.call = cb_stop, .next = NULL, .priority = 1};
    struct NotifierBlock d = {.call = cb_ok, .next = NULL, .priority = 1};

    /* Build list a->b->c->d using explicit linking for test clarity */
    nh.head = &a;
    a.next = &b;
    b.next = &c;
    c.next = &d;
    d.next = NULL;

    /* 1) Call with nr_to_call = 4, expect stop at c and ret has STOP mask */
    int count = 0;
    int expected_stop = NOTIFY_STOP_MASK;
    int nr_calls = 0;
    int ret = NotifierCallChain(&nh.head, 0, &count, 4, &nr_calls);
    print_case_result("stop returns NOTIFY_STOP", expected_stop, ret & NOTIFY_STOP_MASK, &pass, &caseIdx);
    print_case_result("nr_calls==3 (a,b,c)", 3, nr_calls, &pass, &caseIdx);

    /* 2) Call with nr_to_call = 2, expect two calls and NOTIFY_DONE from last return (b returns DONE) */
    nr_calls = 0;
    ret = NotifierCallChain(&nh.head, 0, NULL, 2, &nr_calls);
    print_case_result("nr_calls==2", 2, nr_calls, &pass, &caseIdx);
    print_case_result("ret==NOTIFY_DONE (from b)", NOTIFY_DONE, ret, &pass, &caseIdx);

    /* 3) Verify v is passed and incremented by cb_count */
    struct Ctx ctx = {0};
    a.call = cb_count;
    b.call = cb_count;
    c.call = cb_count;
    d.call = cb_count;
    nr_calls = 0;
    ret = NotifierCallChain(&nh.head, 123, &ctx, 4, &nr_calls);
    print_case_result("ctx hits==4", 4, ctx.hits, &pass, &caseIdx);

    double rate = 100.0 * pass / total;
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}

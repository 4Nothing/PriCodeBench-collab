#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "auto_stub.h"
#include "../include/prt_timer_external.h"

/* StubResetTask is defined in stub_prt_signal_env.c */
extern void StubResetTask(void);


static int g_total = 0;
static int g_pass = 0;

/* Counter reset macro */
#define RESET_COUNTERS() do { g_total = 0; g_pass = 0; } while(0)

/* Add pending signal helper - must add sigInfoNode to sigInfoList */
static inline void add_pending_signal(struct TagTskCb *task, int signum)
{
    task->sigPending |= sigMask(signum);
    /* Allocate and add sigInfoNode to sigInfoList */
    sigInfoNode *node = (sigInfoNode *)malloc(sizeof(sigInfoNode));
    if (node) {
        node->siginfo.si_signo = signum;
        ListTailAdd(&node->siglist, &task->sigInfoList);
    }
}

static void print_result(const char *case_name, U32 expected, U32 actual)
{
    printf("[CASE] %s\n", case_name);
    printf("  Expected: 0x%08X\n", expected);
    printf("  Actual  : 0x%08X\n", actual);
    int ok = (expected == actual);
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL");
    g_total++;
    g_pass += ok;
}

static void print_result_int(const char *case_name, int expected, int actual)
{
    printf("[CASE] %s\n", case_name);
    printf("  Expected: %d\n", expected);
    printf("  Actual  : %d\n", actual);
    int ok = (expected == actual);
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL");
    g_total++;
    g_pass += ok;
}

static void print_result_ptr(const char *case_name, void *expected, void *actual)
{
    printf("[CASE] %s\n", case_name);
    printf("  Expected: %p\n", expected);
    printf("  Actual  : %p\n", actual);
    int ok = (expected == actual);
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL");
    g_total++;
    g_pass += ok;
}

static void print_pass_rate()
{
    double rate = g_total ? (100.0 * g_pass / g_total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, g_pass, g_total);
}

static int print_summary()
{
    double rate = g_total ? (100.0 * g_pass / g_total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, g_pass, g_total);
    return (g_pass == g_total) ? 0 : 1;
}

#endif // TEST_COMMON_H

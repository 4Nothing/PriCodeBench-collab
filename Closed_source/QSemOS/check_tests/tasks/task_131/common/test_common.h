#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "auto_stub.h"
// REMOVED: ../include/prt_timer_external.h not available

/* Forward declarations for signal inline functions that need external definitions */
extern inline U32 OsAddSignalPendingFlag(struct TagTskCb *taskCb, signalInfo *info);
extern inline void OsSignalWaitReSche(struct TagTskCb *taskCb, U32 taskStatus);
extern inline U32 OsSigWaitProcPendingSignal(struct TagTskCb *runTsk, const signalSet *set, signalInfo *info);
extern inline U32 OsSignalWaitSche(struct TagTskCb *runTsk, const signalSet *set, U32 timeOutTick);
extern inline void OsHandleOneSignal(struct TagTskCb *runTsk, int signum);
extern inline void OsSignalTimeOutSet(struct TagTskCb *runTsk, U32 timeOutTick);

static int g_total = 0;
static int g_pass = 0;

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

static void RESET_COUNTERS(void)
{
    g_total = 0;
    g_pass = 0;
}

static void print_result_int(const char *case_name, U32 expected, U32 actual)
{
    print_result(case_name, expected, actual);
}

static void print_pass_rate(void)
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

/* Test framework helpers for task tests */
static inline void test_reset_stats(void)
{
    RESET_COUNTERS();
}

static inline void test_print_summary(void)
{
    print_summary();
}

/* EXPECT macros for test assertions */
#define EXPECT_BOOL_EQ(name, expected, actual) do { \
    g_total++; \
    int ok = ((expected) == (actual)); \
    printf("[%s] %s: expected=%s, actual=%s -> %s\n", \
           name, ok ? "PASS" : "FAIL", \
           (expected) ? "true" : "false", \
           (actual) ? "true" : "false", \
           ok ? "PASS" : "FAIL"); \
    g_pass += ok; \
} while (0)

#define EXPECT_INT_EQ(name, expected, actual) do { \
    g_total++; \
    int ok = ((expected) == (actual)); \
    printf("[%s] %s: expected=%d, actual=%d -> %s\n", \
           name, ok ? "PASS" : "FAIL", \
           (int)(expected), (int)(actual), \
           ok ? "PASS" : "FAIL"); \
    g_pass += ok; \
} while (0)

#endif // TEST_COMMON_H

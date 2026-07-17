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

#define test_reset_stats() RESET_COUNTERS()

#define EXPECT_BOOL_EQ(msg, expected, actual) do { \
    int exp = (expected); int act = (actual); \
    int ok = (exp == act); \
    printf("[CASE] %s\n", msg); \
    printf("  Expected: %s\n", exp ? "true" : "false"); \
    printf("  Actual  : %s\n", act ? "true" : "false"); \
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL"); \
    g_total++; g_pass += ok; \
} while(0)

#define EXPECT_INT_EQ(msg, expected, actual) do { \
    int exp = (expected); int act = (actual); \
    int ok = (exp == act); \
    printf("[CASE] %s\n", msg); \
    printf("  Expected: %d\n", exp); \
    printf("  Actual  : %d\n", act); \
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL"); \
    g_total++; g_pass += ok; \
} while(0)

#define EXPECT_UINT_EQ(msg, expected, actual) do { \
    unsigned int exp = (expected); unsigned int act = (actual); \
    int ok = (exp == act); \
    printf("[CASE] %s\n", msg); \
    printf("  Expected: %u\n", exp); \
    printf("  Actual  : %u\n", act); \
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL"); \
    g_total++; g_pass += ok; \
} while(0)

#define test_print_summary() print_summary()

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

#endif // TEST_COMMON_H
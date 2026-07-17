#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "auto_stub.h"

/* StubResetTask is defined in stub_prt_signal_env.c */
extern void StubResetTask(void);

static int g_total = 0;
static int g_pass = 0;

/* Counter reset macro */
#define RESET_COUNTERS() do { g_total = 0; g_pass = 0; } while(0)

void print_result(const char *case_name, U32 expected, U32 actual)
{
    printf("[CASE] %s\n", case_name);
    printf("  Expected: 0x%08X\n", expected);
    printf("  Actual  : 0x%08X\n", actual);
    int ok = (expected == actual);
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL");
    g_total++;
    g_pass += ok;
}

void print_result_int(const char *case_name, int expected, int actual)
{
    printf("[CASE] %s\n", case_name);
    printf("  Expected: %d\n", expected);
    printf("  Actual  : %d\n", actual);
    int ok = (expected == actual);
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL");
    g_total++;
    g_pass += ok;
}

void print_pass_rate(void)
{
    double rate = g_total ? (100.0 * g_pass / g_total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, g_pass, g_total);
}

int print_summary()
{
    double rate = g_total ? (100.0 * g_pass / g_total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, g_pass, g_total);
    return (g_pass == g_total) ? 0 : 1;
}

#endif // TEST_COMMON_H

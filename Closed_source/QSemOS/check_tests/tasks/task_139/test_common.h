#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "auto_stub.h"
// REMOVED bad include


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

static int print_summary()
{
    double rate = g_total ? (100.0 * g_pass / g_total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, g_pass, g_total);
    return (g_pass == g_total) ? 0 : 1;
}

/* print_case macro for task tests */
#define print_case(name, expected, actual) do { \
    g_total++; \
    int ok = (expected == actual); \
    g_pass += ok; \
    printf("[CASE] %s\n", name); \
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL"); \
} while(0)

/* print_rate macro - calls print_summary */
#define print_rate() print_summary()

#endif // TEST_COMMON_H

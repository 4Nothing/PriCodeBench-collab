#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>

static int g_total = 0;
static int g_pass = 0;

static void print_result(const char *case_name, uint32_t expected, uint32_t actual)
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

static void print_case(const char *name, const char *expected, const char *actual, int passed)
{
    printf("[CASE] %s\n", name);
    printf("  Expected: %s\n", expected);
    printf("  Actual  : %s\n", actual);
    printf("  Result  : %s\n\n", passed ? "PASS" : "FAIL");
}

static void print_pass_rate(int pass, int total)
{
    double rate = total ? (100.0 * pass / total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, pass, total);
}

#endif // TEST_COMMON_H

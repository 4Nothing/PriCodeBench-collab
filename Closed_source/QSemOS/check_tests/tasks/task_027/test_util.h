#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int TU_total = 0;
static int TU_pass = 0;

static void TU_reset(void)
{
    TU_total = 0;
    TU_pass = 0;
}

static void TU_check_u32(const char *caseName, unsigned expected, unsigned actual)
{
    ++TU_total;
    int ok = (expected == actual);
    if (ok)
        ++TU_pass;
    printf("[CASE] %s\n  expected=0x%X\n  actual  =0x%X\n  result  =%s\n\n", caseName, expected, actual, ok ? "PASS" : "FAIL");
}

static void TU_check_u64(const char *caseName, unsigned long long expected, unsigned long long actual)
{
    ++TU_total;
    int ok = (expected == actual);
    if (ok)
        ++TU_pass;
    printf("[CASE] %s\n  expected=%llu\n  actual  =%llu\n  result  =%s\n\n", caseName, expected, actual, ok ? "PASS" : "FAIL");
}

static void TU_check_bool(const char *caseName, int expected, int actual)
{
    ++TU_total;
    int ok = ((expected != 0) == (actual != 0));
    if (ok)
        ++TU_pass;
    printf("[CASE] %s\n  expected=%s\n  actual  =%s\n  result  =%s\n\n", caseName, expected ? "true" : "false", actual ? "true" : "false", ok ? "PASS" : "FAIL");
}

static void TU_check_ptr(const char *caseName, const void *expected, const void *actual)
{
    ++TU_total;
    int ok = (expected == actual);
    if (ok)
        ++TU_pass;
    printf("[CASE] %s\n  expected=%p\n  actual  =%p\n  result  =%s\n\n", caseName, expected, actual, ok ? "PASS" : "FAIL");
}

static void TU_report(void)
{
    double rate = TU_total ? (100.0 * (double)TU_pass / (double)TU_total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, TU_pass, TU_total);
}

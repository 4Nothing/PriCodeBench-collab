#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include "auto_stub.h"

static inline void print_case_u32(const char *name, U32 expected, U32 actual, int *pass, int *total)
{
    (*total)++;
    bool ok = (expected == actual);
    if (ok) (*pass)++;
    printf("[CASE] %s\n  Expected: %u\n  Actual  : %u\n  Result  : %s\n\n", name, (unsigned)expected, (unsigned)actual, ok ? "PASS" : "FAIL");
}

static inline void print_case_bool(const char *name, bool expected, bool actual, int *pass, int *total)
{
    (*total)++;
    bool ok = (expected == actual);
    if (ok) (*pass)++;
    printf("[CASE] %s\n  Expected: %s\n  Actual  : %s\n  Result  : %s\n\n", name, expected ? "true" : "false", actual ? "true" : "false", ok ? "PASS" : "FAIL");
}

static inline void print_pass_rate(int pass, int total)
{
    double rate = (total == 0) ? 0.0 : (100.0 * (double)pass / (double)total);
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, pass, total);
}

#endif

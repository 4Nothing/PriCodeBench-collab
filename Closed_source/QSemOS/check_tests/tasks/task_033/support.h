// Common support for unit tests
#ifndef SUPPORT_H
#define SUPPORT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Test utilities
static inline void print_case_u32(const char *name, unsigned int expected, unsigned int actual, int *pass, int *total)
{
    (*total)++;
    printf("[CASE] %s\n", name);
    printf("  Expected: %u\n", expected);
    printf("  Actual  : %u\n", actual);
    if (expected == actual) { (*pass)++; printf("  Result  : PASS\n\n"); }
    else { printf("  Result  : FAIL\n\n"); }
}

static inline void print_case_bool(const char *name, bool expected, bool actual, int *pass, int *total)
{
    (*total)++;
    printf("[CASE] %s\n", name);
    printf("  Expected: %s\n", expected ? "true" : "false");
    printf("  Actual  : %s\n", actual ? "true" : "false");
    if (expected == actual) { (*pass)++; printf("  Result  : PASS\n\n"); }
    else { printf("  Result  : FAIL\n\n"); }
}

static inline void print_pass_rate(int pass, int total)
{
    double rate = total ? (100.0 * pass / total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, pass, total);
}

#endif

#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include "common.h"
#include <stdio.h>

/* Tiny test harness helpers */
static int assert_eq_u32(const char *name, U32 expect, U32 actual)
{
    printf("[CASE] %s\n  Expect: %u\n  Actual: %u\n", name, expect, actual);
    return expect == actual;
}

static void print_pass_rate(int pass, int total)
{
    double rate = total ? (100.0 * pass / total) : 0.0;
    printf("Pass-Rate: %.2f%%\n", rate);
}

#endif /* TEST_SUPPORT_H */

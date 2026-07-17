#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Basic types - use same definitions as src to avoid conflicts
typedef unsigned int U32;
typedef unsigned long long U64;

// TU test framework
static int TU_total = 0;
static int TU_pass = 0;

static void TU_reset(void)
{
    TU_total = 0;
    TU_pass = 0;
}

static void TU_EXPECT_EQ_U32(U32 expected, U32 actual, const char *name)
{
    ++TU_total;
    int ok = (expected == actual);
    if (ok) ++TU_pass;
    printf("[%s] expect=%u actual=%u -> %s\n", name, expected, actual, ok ? "PASS" : "FAIL");
}

static void TU_EXPECT_TRUE(int condition, const char *name)
{
    ++TU_total;
    int ok = (condition != 0);
    if (ok) ++TU_pass;
    printf("[%s] -> %s\n", name, ok ? "PASS" : "FAIL");
}

static void TU_report(void)
{
    double rate = TU_total ? (100.0 * (double)TU_pass / (double)TU_total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, TU_pass, TU_total);
}
#include <stdio.h>
#include <stdint.h>

#include "prt_attr_external.h"
#include "prt_sys_external.h"

volatile U64 g_uniTicks = 0;
volatile U64 g_ticksOffset = 0;
volatile U64 g_systemClock = 1000000ULL;

#include "func_under_test.c"

typedef struct
{
    const char *name;
    U64 uni;
    U64 off;
    U64 expected;
    U64 actual;
    int pass;
} CaseResTick;

int main(void)
{
    CaseResTick results[6];
    int idx = 0, passCnt = 0;

    /* Case 1: both zero */
    g_uniTicks = 0;
    g_ticksOffset = 0;
    results[idx].name = "0 + 0 = 0";
    results[idx].uni = g_uniTicks;
    results[idx].off = g_ticksOffset;
    results[idx].expected = 0ULL;
    results[idx].actual = PRT_TickGetCount();
    results[idx].pass = (results[idx].actual == results[idx].expected);
    passCnt += results[idx].pass;
    idx++;

    /* Case 2: simple sum */
    g_uniTicks = 100;
    g_ticksOffset = 23;
    results[idx].name = "100 + 23 = 123";
    results[idx].uni = g_uniTicks;
    results[idx].off = g_ticksOffset;
    results[idx].expected = 123ULL;
    results[idx].actual = PRT_TickGetCount();
    results[idx].pass = (results[idx].actual == results[idx].expected);
    passCnt += results[idx].pass;
    idx++;

    /* Case 3: large numbers without overflow */
    g_uniTicks = 0xFFFFFFFFULL;
    g_ticksOffset = 1;
    results[idx].name = "0xFFFFFFFF + 1 = 0x100000000";
    results[idx].uni = g_uniTicks;
    results[idx].off = g_ticksOffset;
    results[idx].expected = 0x100000000ULL;
    results[idx].actual = PRT_TickGetCount();
    results[idx].pass = (results[idx].actual == results[idx].expected);
    passCnt += results[idx].pass;
    idx++;

    /* Case 4: both large 64-bit */
    g_uniTicks = 0xFFFFFFFFFFFFFFF0ULL;
    g_ticksOffset = 0x0FULL;
    results[idx].name = "near wrap add";
    results[idx].uni = g_uniTicks;
    results[idx].off = g_ticksOffset;
    results[idx].expected = 0xFFFFFFFFFFFFFFFFULL;
    results[idx].actual = PRT_TickGetCount();
    results[idx].pass = (results[idx].actual == results[idx].expected);
    passCnt += results[idx].pass;
    idx++;

    /* Case 5: wrap to zero (natural unsigned wrap) */
    g_uniTicks = 0xFFFFFFFFFFFFFFFFULL;
    g_ticksOffset = 1ULL;
    results[idx].name = "wrap to 0";
    results[idx].uni = g_uniTicks;
    results[idx].off = g_ticksOffset;
    results[idx].expected = 0ULL; /* unsigned wrap */
    results[idx].actual = PRT_TickGetCount();
    results[idx].pass = (results[idx].actual == results[idx].expected);
    passCnt += results[idx].pass;
    idx++;

    /* Case 6: random values */
    g_uniTicks = 123456789ULL;
    g_ticksOffset = 987654321ULL;
    results[idx].name = "random add";
    results[idx].uni = g_uniTicks;
    results[idx].off = g_ticksOffset;
    results[idx].expected = 1111111110ULL;
    results[idx].actual = PRT_TickGetCount();
    results[idx].pass = (results[idx].actual == results[idx].expected);
    passCnt += results[idx].pass;
    idx++;

    for (int i = 0; i < idx; ++i)
    {
        printf("[Case %d] %s\n", i + 1, results[i].name);
        printf("  Expected: %llu\n", (unsigned long long)results[i].expected);
        printf("  Actual  : %llu\n", (unsigned long long)results[i].actual);
    }
    double passRate = (idx == 0) ? 0.0 : (100.0 * (double)passCnt / (double)idx);
    printf("Pass-Rate: %.2f%%\n", passRate);
    return (passCnt == idx) ? 0 : 1;
}

#include <stdio.h>
#include <stdint.h>

#include "prt_attr_external.h"
#include "prt_sys_external.h"

volatile U64 g_uniTicks = 0;
volatile U64 g_ticksOffset = 0;
volatile U64 g_systemClock = 1000000ULL; /* default 1 MHz */

#include "func_under_test.c"

static U64 nowVal;
static int reads;
static U64 stepClock(void)
{
    reads++;
    return nowVal++;
}

typedef struct
{
    const char *name;
    U64 clk;
    U32 us;
    U64 expectedCycles;
} CaseUs;

int main(void)
{
    CaseUs cases[] = {
        {"1MHz, 0us -> 0 cycles", 1000000ULL, 0U, 0ULL},
        {"1MHz, 1us -> 1 cycle", 1000000ULL, 1U, 1ULL},
        {"1MHz, 50us -> 50 cycles", 1000000ULL, 50U, 50ULL},
        {"2MHz, 10us -> 20 cycles", 2000000ULL, 10U, 20ULL},
        {"48MHz, 5us -> 240 cycles", 48000000ULL, 5U, 240ULL},
        {"3Hz (low), 1000000us -> 3 cycles", 3ULL, 1000000U, 3ULL}};
    int total = sizeof(cases) / sizeof(cases[0]);
    int passCnt = 0;

    for (int i = 0; i < total; ++i)
    {
        g_systemClock = cases[i].clk;
        g_sysTimeHook = stepClock;
        nowVal = 0;
        reads = 0;

        PRT_ClkDelayUs(cases[i].us);
        U64 actualCyclesReached = nowVal;

        int ok = (actualCyclesReached >= cases[i].expectedCycles);

        printf("[Case %d] %s\n", i + 1, cases[i].name);
        printf("  Expected: cycles=%llu\n", (unsigned long long)cases[i].expectedCycles);
        printf("  Actual  : cyclesReached=%llu, reads=%d\n", (unsigned long long)actualCyclesReached, reads);

        passCnt += ok;
    }

    double passRate = (total == 0) ? 0.0 : (100.0 * (double)passCnt / (double)total);
    printf("Pass-Rate: %.2f%%\n", passRate);
    return (passCnt == total) ? 0 : 1;
}

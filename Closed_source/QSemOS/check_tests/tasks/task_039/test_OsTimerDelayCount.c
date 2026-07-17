#include <stdio.h>
#include <stdint.h>

#include "prt_attr_external.h"
#include "prt_sys_external.h"

volatile U64 g_uniTicks = 0;
volatile U64 g_ticksOffset = 0;
volatile U64 g_systemClock = 1000000ULL;

#include "func_under_test.c"

/* A controllable clock for simulating time progression */
static U64 fakeNow = 0;
static int readCount = 0;
static U64 fakeClock(void)
{
    readCount++;
    return fakeNow;
}

typedef struct
{
    const char *name;
    U64 start;
    U64 cycles;
    U64 advances[4];
    int advCount;
    int minReads;
    int pass;
} CaseDelay;

/* Hook increments internal counter on each call, ensuring it will eventually reach end. */
static U64 stepClock(void)
{
    readCount++;
    return fakeNow++;
}

int main(void)
{
    int passCnt = 0, idx = 0;
    struct
    {
        const char *name;
        U64 start;
        U64 cycles;
    } cases[6] = {
        {"no wait when cycles=0", 1000ULL, 0ULL},
        {"small wait 5 cycles", 200ULL, 5ULL},
        {"larger wait 100 cycles", 0ULL, 100ULL},
        {"near-maximum start small wait (wrap)", 0xFFFFFFFFFFFFFFF0ULL, 0x10ULL},
        {"near-maximum start 1 cycle (wrap)", 0xFFFFFFFFFFFFFFFFULL, 1ULL},
        {"one cycle", 500ULL, 1ULL}};

    for (int i = 0; i < 6; ++i)
    {
        readCount = 0;
        g_sysTimeHook = stepClock;
        fakeNow = cases[i].start;
        U64 before = fakeNow;
        U64 end = before + cases[i].cycles;
        OsTimerDelayCount(cases[i].cycles);
        U64 after = fakeNow;

        int ok;
        if (cases[i].cycles == 0ULL)
        {
            ok = (readCount >= 1);
        }
        else
        {
            ok = (after >= end);
        }

        printf("[Case %d] %s\n", i + 1, cases[i].name);
        printf("  Expected: end=%llu\n", (unsigned long long)end);
        printf("  Actual  : start=%llu, cycles=%llu, reads=%d, finalNow=%llu\n",
               (unsigned long long)before,
               (unsigned long long)cases[i].cycles,
               readCount,
               (unsigned long long)after);

        passCnt += ok;
        idx++;
    }

    double passRate = (idx == 0) ? 0.0 : (100.0 * (double)passCnt / (double)idx);
    printf("Pass-Rate: %.2f%%\n", passRate);
    return (passCnt == idx) ? 0 : 1;
}

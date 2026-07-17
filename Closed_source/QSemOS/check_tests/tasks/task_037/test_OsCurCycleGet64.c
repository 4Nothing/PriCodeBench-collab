#include <stdio.h>
#include <stdint.h>

#include "prt_attr_external.h"
#include "prt_sys_external.h"

/* Externs definitions */
volatile U64 g_uniTicks = 0;
volatile U64 g_ticksOffset = 0;
volatile U64 g_systemClock = 1000000ULL;

/* Include code under test */
#include "func_under_test.c"

static U64 fixedHookA(void) { return 0ULL; }
static U64 fixedHookB(void) { return 42ULL; }
static U64 incHook(void)
{
    static U64 v = 1000ULL;
    return v++;
}
static U64 largeHook(void) { return 0xFFFFFFFFFFFFFFF0ULL; }

typedef struct
{
    const char *name;
    U64 expected;
    U64 actual;
    int pass;
} CaseRes64;

int main(void)
{
    CaseRes64 results[6];
    int idx = 0, passCnt = 0;

    g_sysTimeHook = NULL;

    /* Case 1: NULL hook -> returns 0 */
    results[idx].name = "NULL hook returns 0";
    results[idx].expected = 0ULL;
    results[idx].actual = OsCurCycleGet64();
    results[idx].pass = (results[idx].actual == results[idx].expected);
    passCnt += results[idx].pass;
    idx++;

    /* Case 2: fixed 0 */
    g_sysTimeHook = fixedHookA;
    results[idx].name = "fixed 0";
    results[idx].expected = 0ULL;
    results[idx].actual = OsCurCycleGet64();
    results[idx].pass = (results[idx].actual == results[idx].expected);
    passCnt += results[idx].pass;
    idx++;

    /* Case 3: fixed 42 */
    g_sysTimeHook = fixedHookB;
    results[idx].name = "fixed 42";
    results[idx].expected = 42ULL;
    results[idx].actual = OsCurCycleGet64();
    results[idx].pass = (results[idx].actual == results[idx].expected);
    passCnt += results[idx].pass;
    idx++;

    /* Case 4: incrementing hook, first call 1000 */
    g_sysTimeHook = incHook;
    results[idx].name = "inc hook first 1000";
    results[idx].expected = 1000ULL;
    results[idx].actual = OsCurCycleGet64();
    results[idx].pass = (results[idx].actual == results[idx].expected);
    passCnt += results[idx].pass;
    idx++;

    /* Case 5: incrementing hook, second call 1001 */
    results[idx].name = "inc hook second 1001";
    results[idx].expected = 1001ULL;
    results[idx].actual = OsCurCycleGet64();
    results[idx].pass = (results[idx].actual == results[idx].expected);
    passCnt += results[idx].pass;
    idx++;

    /* Case 6: large value */
    g_sysTimeHook = largeHook;
    results[idx].name = "large near max";
    results[idx].expected = 0xFFFFFFFFFFFFFFF0ULL;
    results[idx].actual = OsCurCycleGet64();
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

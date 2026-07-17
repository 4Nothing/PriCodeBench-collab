#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "prt_attr_external.h"
#include "prt_sys_external.h"

/* Provide definitions for externs */
volatile U64 g_uniTicks = 0;
volatile U64 g_ticksOffset = 0;
volatile U64 g_systemClock = 1000000ULL; /* 1 MHz default for conversion helpers */

/* Bring in code under test */
#include "func_under_test.c"

static U64 dummyHook1(void) { return 1234ULL; }
static U64 dummyHook2(void) { return 5678ULL; }

typedef struct
{
    const char *name;
    U32 expected;
    U32 actual;
    int pass;
} CaseRes;

int main(void)
{
    CaseRes results[6];
    int idx = 0;
    int passCnt = 0;

    /* Reset globals */
    g_sysTimeHook = NULL;

    /* Case 1: Register non-null hook first time -> OS_OK */
    results[idx].name = "first register hook -> OS_OK";
    results[idx].expected = OS_OK;
    results[idx].actual = OsSetSysTimeHook(dummyHook1);
    results[idx].pass = (results[idx].actual == results[idx].expected);
    passCnt += results[idx].pass;
    idx++;

    /* Case 2: Re-register any hook (same) -> repeated error */
    results[idx].name = "register same hook again -> REPEATED";
    results[idx].expected = OS_ERRNO_SYS_TIME_HOOK_REGISTER_REPEATED;
    results[idx].actual = OsSetSysTimeHook(dummyHook1);
    results[idx].pass = (results[idx].actual == results[idx].expected);
    passCnt += results[idx].pass;
    idx++;

    /* Case 3: Re-register different hook -> repeated error */
    results[idx].name = "register different hook again -> REPEATED";
    results[idx].expected = OS_ERRNO_SYS_TIME_HOOK_REGISTER_REPEATED;
    results[idx].actual = OsSetSysTimeHook(dummyHook2);
    results[idx].pass = (results[idx].actual == results[idx].expected);
    passCnt += results[idx].pass;
    idx++;

    /* For further cases, simulate reset by clearing global and registering null */
    g_sysTimeHook = NULL;

    /* Case 4: Register NULL hook is allowed (per code) -> OS_OK and sets NULL */
    results[idx].name = "register NULL hook -> OS_OK";
    results[idx].expected = OS_OK;
    results[idx].actual = OsSetSysTimeHook(NULL);
    results[idx].pass = (results[idx].actual == results[idx].expected && g_sysTimeHook == NULL);
    passCnt += results[idx].pass;
    idx++;

    /* Case 5: After NULL registration, registering non-null should fail as repeated? */
    /* According to implementation, registration fails only if g_sysTimeHook != NULL at entry. After previous call, g_sysTimeHook == NULL, so it should succeed. */
    results[idx].name = "after NULL reg, register hook -> OS_OK";
    results[idx].expected = OS_OK;
    results[idx].actual = OsSetSysTimeHook(dummyHook2);
    results[idx].pass = (results[idx].actual == results[idx].expected && g_sysTimeHook == dummyHook2);
    passCnt += results[idx].pass;
    idx++;

    /* Case 6: One more repeated error with non-null present */
    results[idx].name = "re-register with hook present -> REPEATED";
    results[idx].expected = OS_ERRNO_SYS_TIME_HOOK_REGISTER_REPEATED;
    results[idx].actual = OsSetSysTimeHook(dummyHook1);
    results[idx].pass = (results[idx].actual == results[idx].expected);
    passCnt += results[idx].pass;
    idx++;

    /* Print details */
    for (int i = 0; i < idx; ++i)
    {
        printf("[Case %d] %s\n", i + 1, results[i].name);
        printf("  Expected: %u\n", results[i].expected);
        printf("  Actual  : %u\n", results[i].actual);
    }

    double passRate = (idx == 0) ? 0.0 : (100.0 * (double)passCnt / (double)idx);
    printf("Pass-Rate: %.2f%%\n", passRate);

    return (passCnt == idx) ? 0 : 1;
}

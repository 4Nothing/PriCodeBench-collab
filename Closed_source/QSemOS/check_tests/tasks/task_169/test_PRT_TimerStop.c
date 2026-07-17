#include <stdio.h>
#include <stdlib.h>
#include "test_shims_common.h"
#include "prt_timer_external.h"
#include "prt_err_external.h"
#include "prt_attr_external.h"

#include "func_under_test.c"
TimerApi g_timerApi[4];

/* Bring in the implementation under test */

/* Helpers to simulate different behaviors */
static U32 stop_ok(TimerHandle h)
{
    (void)h;
    return 0;
}
static U32 stop_fail(TimerHandle h)
{
    (void)h;
    return 5678;
}

static void reset_api(void)
{
    for (int i = 0; i < 4; ++i)
    {
        g_timerApi[i].startTimer = NULL;
        g_timerApi[i].stopTimer = NULL;
        g_timerApi[i].timerQuery = NULL;
    }
}

static int run_case(const char *name, U32 mid, TimerHandle h, U32 expect)
{
    U32 actual = PRT_TimerStop(mid, h);
    int pass = (actual == expect);
    printf("[Stop] %s\n  Expect: %u\n  Actual: %u\n  %s\n\n", name, expect, actual, pass ? "PASS" : "FAIL");
    return pass;
}

int main(void)
{
    int passCnt = 0, total = 0;

    reset_api();
    /* Case 1: invalid handle type -> OS_ERRNO_TIMER_HANDLE_INVALID */
    {
        TimerHandle h = make_handle(TIMER_TYPE_INVALID, 1);
        total++;
        passCnt += run_case("invalid handle type", 0, h, OS_ERRNO_TIMER_HANDLE_INVALID);
    }

    /* Case 2: valid type but stopTimer is NULL -> NOT_INIT error */
    {
        reset_api();
        TimerHandle h = make_handle(0, 2);
        total++;
        passCnt += run_case("stop hook NULL", 0, h, OS_ERRNO_TIMER_NOT_INIT_OR_GROUP_NOT_CREATED);
    }

    /* Case 3: stopTimer returns success (0) */
    {
        reset_api();
        g_timerApi[1].stopTimer = stop_ok;
        TimerHandle h = make_handle(1, 3);
        total++;
        passCnt += run_case("stop ok", 0, h, 0);
    }

    /* Case 4: stopTimer returns non-zero error and should be propagated */
    {
        reset_api();
        g_timerApi[2].stopTimer = stop_fail;
        TimerHandle h = make_handle(2, 4);
        total++;
        passCnt += run_case("stop returns error", 0, h, 5678);
    }

    /* Case 5: different mid should be ignored (mid unused) */
    {
        reset_api();
        g_timerApi[3].stopTimer = stop_ok;
        TimerHandle h = make_handle(3, 5);
        total++;
        passCnt += run_case("mid ignored, still ok", 123, h, 0);
    }

    double rate = total ? (100.0 * passCnt / total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, passCnt, total);
    return (passCnt == total) ? 0 : 1;
}

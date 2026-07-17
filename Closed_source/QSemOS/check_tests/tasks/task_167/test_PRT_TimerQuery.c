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
static U32 tq_ok(TimerHandle h, U32 *out)
{
    (void)h;
    if (out)
        *out = 42;
    return 0;
}
static U32 tq_custom(TimerHandle h, U32 *out)
{
    (void)h;
    if (out)
        *out = 7;
    return 900;
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

static int run_case_ptrnull(void)
{
    U32 actual = PRT_TimerQuery(0, make_handle(0, 1), NULL);
    int pass = (actual == OS_ERRNO_TIMER_INPUT_PTR_NULL);
    printf("[Query] expireTime NULL\n  Expect: %u\n  Actual: %u\n  %s\n\n", OS_ERRNO_TIMER_INPUT_PTR_NULL, actual, pass ? "PASS" : "FAIL");
    return pass;
}

static int run_case(const char *name, U32 mid, TimerHandle h, U32 expectRet, U32 expectOut, int checkOut)
{
    U32 out = 0xDEADBEEF;
    U32 actual = PRT_TimerQuery(mid, h, &out);
    int pass = (actual == expectRet) && (!checkOut || (out == expectOut));
    printf("[Query] %s\n  Expect: ret=%u, out=%s%u\n  Actual: ret=%u, out=%u\n  %s\n\n",
           name, expectRet, checkOut ? "" : "(skip)", expectOut, actual, out, pass ? "PASS" : "FAIL");
    return pass;
}

int main(void)
{
    int passCnt = 0, total = 0;

    reset_api();
    /* Case 1: NULL pointer check */
    total++;
    passCnt += run_case_ptrnull();

    /* Case 2: invalid handle type -> HANDLE_INVALID */
    {
        TimerHandle h = make_handle(TIMER_TYPE_INVALID, 1);
        U32 dummy = 0;
        (void)dummy;
        total++;
        passCnt += run_case("invalid handle type", 0, h, OS_ERRNO_TIMER_HANDLE_INVALID, 0, 0);
    }

    /* Case 3: valid type but timerQuery is NULL -> NOT_INIT error */
    {
        reset_api();
        TimerHandle h = make_handle(0, 2);
        total++;
        passCnt += run_case("query hook NULL", 0, h, OS_ERRNO_TIMER_NOT_INIT_OR_GROUP_NOT_CREATED, 0, 0);
    }

    /* Case 4: timerQuery returns success and writes value */
    {
        reset_api();
        g_timerApi[1].timerQuery = tq_ok;
        TimerHandle h = make_handle(1, 3);
        total++;
        passCnt += run_case("query ok", 0, h, 0, 42, 1);
    }

    /* Case 5: timerQuery returns error, value still set */
    {
        reset_api();
        g_timerApi[2].timerQuery = tq_custom;
        TimerHandle h = make_handle(2, 4);
        total++;
        passCnt += run_case("query error with out", 0, h, 900, 7, 1);
    }

    /* Case 6: different mid ignored */
    {
        reset_api();
        g_timerApi[3].timerQuery = tq_ok;
        TimerHandle h = make_handle(3, 5);
        total++;
        passCnt += run_case("mid ignored", 88, h, 0, 42, 1);
    }

    double rate = total ? (100.0 * passCnt / total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, passCnt, total);
    return (passCnt == total) ? 0 : 1;
}

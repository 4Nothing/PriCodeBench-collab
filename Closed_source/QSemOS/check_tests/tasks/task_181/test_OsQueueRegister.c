#include <stdio.h>
#include <stdlib.h>

/* Include test support before the C file under test */
#include "test_support.h"
#include "func_under_test.c"

static int tests_run = 0;
static int tests_passed = 0;

static void reset_globals(void)
{
    g_maxQueue = 0;
    g_allQueue = NULL;
}

static void assert_eq_u32(const char *name, U32 exp, U32 got)
{
    tests_run++;
    if (exp == got)
    {
        tests_passed++;
    }
    else
    {
        printf("[FAIL] %s: expected=%u got=%u\n", name, exp, got);
    }
}

static void assert_true(const char *name, int cond)
{
    tests_run++;
    if (cond)
    {
        tests_passed++;
    }
    else
    {
        printf("[FAIL] %s\n", name);
    }
}

int main(void)
{
    reset_globals();

    /* Case 1: maxQueue == 0 => error */
    U32 ret = OsQueueRegister(0);
    assert_eq_u32("maxQueue==0 returns error", OS_ERRNO_QUEUE_MAXNUM_ZERO, ret);

    /* Case 2: normal small value */
    reset_globals();
    ret = OsQueueRegister(1);
    assert_eq_u32("register ok", OS_OK, ret);
    assert_true("g_maxQueue updated to 1", g_maxQueue == 1);

    /* Case 3: larger value */
    reset_globals();
    ret = OsQueueRegister(100);
    assert_eq_u32("register 100 ok", OS_OK, ret);
    assert_true("g_maxQueue==100", g_maxQueue == 100);

    /* Case 4: boundary 0xFFFF */
    reset_globals();
    ret = OsQueueRegister(0xFFFF);
    assert_eq_u32("register 0xFFFF ok", OS_OK, ret);
    assert_true("g_maxQueue==0xFFFF", g_maxQueue == 0xFFFF);

    /* Case 5: overwrite previous value */
    ret = OsQueueRegister(10);
    assert_eq_u32("re-register ok", OS_OK, ret);
    assert_true("g_maxQueue==10", g_maxQueue == 10);

    float passRate = tests_run ? (100.0f * tests_passed / tests_run) : 0.0f;
    printf("Pass-Rate: %.2f%% (passed %d / total %d)\n", passRate, tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}

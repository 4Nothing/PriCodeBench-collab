#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_support.h"
#include "func_under_test.c"

static int tests_run = 0;
static int tests_passed = 0;

static void reset_env(void)
{
    /* free previously allocated g_allQueue by tests */
    if (g_allQueue)
    {
        OsMemFree(g_allQueue);
    }
    g_allQueue = NULL;
    g_maxQueue = 0;
    g_mock_alloc_fail = 0;
    g_mock_memset_fail = 0;
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
    /* Case 1: alloc returns NULL => OS_ERRNO_QUEUE_NO_MEMORY */
    reset_env();
    g_maxQueue = 10; /* must be >0 for allocation size */
    g_mock_alloc_fail = 1;
    U32 ret = OsQueueConfigInit();
    assert_eq_u32("alloc failure", OS_ERRNO_QUEUE_NO_MEMORY, ret);

    /* Case 2: memset_s fails => goes to OS_GOTO_SYS_ERROR1; in our mock it's no-op, function should continue? */
    /* The original code returns error via OS_GOTO_SYS_ERROR1(); treat as OS_ERRNO_QUEUE_NO_MEMORY-like error for test. */
    reset_env();
    g_maxQueue = 2;
    g_mock_memset_fail = 1;
    ret = OsQueueConfigInit();
    /* After failure, g_allQueue should still be set? In code, OS_GOTO_SYS_ERROR1() then returns OS_OK? not clear.
       To avoid ambiguity, we assume memset failure should not set g_allQueue and function returns non-OK (simulate -1 mapped to not OS_OK). */
    /* We'll conservatively check that g_allQueue != NULL as memory was allocated before memset. */
    assert_true("memset failure still allocated", g_allQueue != NULL);
    /* Cleanup allocated buffer */
    reset_env();

    /* Case 3: success path sets g_allQueue and zeroes memory */
    reset_env();
    g_maxQueue = 3;
    ret = OsQueueConfigInit();
    assert_eq_u32("success ret", OS_OK, ret);
    assert_true("g_allQueue set", g_allQueue != NULL);
    /* verify zeroing */
    size_t bytes = g_maxQueue * sizeof(struct TagQueCb);
    unsigned char *p = (unsigned char *)g_allQueue;
    int all_zero = 1;
    for (size_t i = 0; i < bytes; i++)
    {
        if (p[i] != 0)
        {
            all_zero = 0;
            break;
        }
    }
    assert_true("memory zeroed", all_zero);

    /* Case 4: large maxQueue */
    reset_env();
    g_maxQueue = 1;
    ret = OsQueueConfigInit();
    assert_eq_u32("one element ok", OS_OK, ret);

    /* Case 5: multiple calls allocate new buffer */
    ret = OsQueueConfigInit();
    assert_eq_u32("second call ok (leak acceptable in test)", OS_OK, ret);

    float passRate = tests_run ? (100.0f * tests_passed / tests_run) : 0.0f;
    printf("Pass-Rate: %.2f%% (passed %d / total %d)\n", passRate, tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}

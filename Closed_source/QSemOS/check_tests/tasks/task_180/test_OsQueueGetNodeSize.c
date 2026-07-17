#include <stdio.h>
#include <stdlib.h>
#include "test_support.h"
#include "func_under_test.c"

static int tests_run = 0;
static int tests_passed = 0;

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

int main(void)
{
    /* Align(1,4)=4 + head 4 = 8 */
    assert_eq_u32("size 1", 8U, OsQueueGetNodeSize(1));

    /* size already aligned */
    assert_eq_u32("size 4", 8U, OsQueueGetNodeSize(4));

    /* size 5 -> align to 8 + head 4 = 12 */
    assert_eq_u32("size 5", 12U, OsQueueGetNodeSize(5));

    /* size 15 -> align to 16 + head 4 = 20 */
    assert_eq_u32("size 15", 20U, OsQueueGetNodeSize(15));

    /* boundary near U16 max: 65535 align to 65536 + 4 = 65540 -> wraps in U32 math fine, but function returns U32 */
    assert_eq_u32("size 65535", ((65535U + 3U) & ~3U) + 4U, OsQueueGetNodeSize(65535));

    float passRate = tests_run ? (100.0f * tests_passed / tests_run) : 0.0f;
    printf("Pass-Rate: %.2f%% (passed %d / total %d)\n", passRate, tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}

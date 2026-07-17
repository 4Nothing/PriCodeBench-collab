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
    U32 tmp;

    /* 1: queueId NULL */
    assert_eq_u32("queueId NULL", OS_ERRNO_QUEUE_CREAT_PTR_NULL, OsQueueCreatParaCheck(1, 1, NULL));

    /* 2: nodeNum==0 */
    assert_eq_u32("nodeNum zero", OS_ERRNO_QUEUE_PARA_ZERO, OsQueueCreatParaCheck(0, 1, &tmp));

    /* 3: nodeSize==0 */
    assert_eq_u32("nodeSize zero", OS_ERRNO_QUEUE_PARA_ZERO, OsQueueCreatParaCheck(1, 0, &tmp));

    /* 4: nodeSize leads to size > OS_MAX_U16 */
    /* Choose nodeSize so that ALIGN(size,4)+4 > 65535 => ALIGN >= 65532 then +4 => 65536+? . Use 65535. */
    assert_eq_u32("nodeSize too big", OS_ERRNO_QUEUE_NSIZE_INVALID, OsQueueCreatParaCheck(1, 65535, &tmp));

    /* 5: OK case */
    assert_eq_u32("ok params", OS_OK, OsQueueCreatParaCheck(5, 8, &tmp));

    /* 6: boundary exactly OS_MAX_U16 when aligned+head */
    /* find size s.t. ALIGN(s,4)+4 == 65535 => ALIGN==65531 impossible; so last valid should be 65531-? We'll just assert it returns invalid for 65531 */
    assert_eq_u32("boundary invalid", OS_ERRNO_QUEUE_NSIZE_INVALID, OsQueueCreatParaCheck(1, 65531, &tmp));

    float passRate = tests_run ? (100.0f * tests_passed / tests_run) : 0.0f;
    printf("Pass-Rate: %.2f%% (passed %d / total %d)\n", passRate, tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}

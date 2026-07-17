#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Include test support before the C file under test */
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

static void prepare_queue_cbs(U16 count)
{
    if (g_allQueue)
    {
        for (U16 i = 0; i < g_maxQueue; ++i)
        {
            if (g_allQueue[i].queue)
            {
                free(g_allQueue[i].queue);
                g_allQueue[i].queue = NULL;
            }
        }
        free(g_allQueue);
    }
    g_maxQueue = count;
    g_allQueue = (struct TagQueCb *)calloc(count, sizeof(struct TagQueCb));
}

int main(void)
{
    U32 ret, qId = 0;

    /* Case 1: para check: queueId NULL */
    ret = PRT_QueueCreate(1, 1, NULL);
    assert_eq_u32("queueId NULL", OS_ERRNO_QUEUE_CREAT_PTR_NULL, ret);

    /* Case 2: para check: zeros */
    ret = PRT_QueueCreate(0, 1, &qId);
    assert_eq_u32("nodeNum zero", OS_ERRNO_QUEUE_PARA_ZERO, ret);
    ret = PRT_QueueCreate(1, 0, &qId);
    assert_eq_u32("nodeSize zero", OS_ERRNO_QUEUE_PARA_ZERO, ret);

    /* Case 3: para check: size too big */
    ret = PRT_QueueCreate(1, 65535, &qId);
    assert_eq_u32("size too big", OS_ERRNO_QUEUE_NSIZE_INVALID, ret);

    /* Case 4: lower layer failure propagates */
    prepare_queue_cbs(0);
    ret = PRT_QueueCreate(1, 4, &qId);
    assert_eq_u32("no CBs -> unavailable", OS_ERRNO_QUEUE_CB_UNAVAILABLE, ret);

    /* Case 5: success path */
    prepare_queue_cbs(2);
    ret = PRT_QueueCreate(2, 4, &qId);
    assert_eq_u32("create ok", OS_OK, ret);
    assert_true("qId valid", qId < g_maxQueue);

    /* Case 6: allocation failure at inner call */
    prepare_queue_cbs(1);
    g_mock_alloc_fail = 1;
    ret = PRT_QueueCreate(1, 4, &qId);
    assert_eq_u32("inner alloc fail", OS_ERRNO_QUEUE_CREATE_NO_MEMORY, ret);

    float passRate = tests_run ? (100.0f * tests_passed / tests_run) : 0.0f;
    printf("Pass-Rate: %.2f%% (passed %d / total %d)\n", passRate, tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}

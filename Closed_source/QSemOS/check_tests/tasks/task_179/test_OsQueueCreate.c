#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static void free_all_resources(void)
{
    if (g_allQueue)
    {
        for (U16 i = 0; i < g_maxQueue; ++i)
        {
            if (g_allQueue[i].queue)
            {
                OsMemFree(g_allQueue[i].queue);
                g_allQueue[i].queue = NULL;
            }
        }
        free(g_allQueue);
        g_allQueue = NULL;
    }
}

static void setup_empty_cb_array(U16 count)
{
    free_all_resources();
    g_maxQueue = count;
    g_allQueue = (struct TagQueCb *)calloc(count, sizeof(struct TagQueCb));
    for (U16 i = 0; i < count; ++i)
    {
        g_allQueue[i].queueState = OS_QUEUE_UNUSED;
    }
}

int main(void)
{
    U32 qId = 0;
    U32 ret;

    /* Case 1: no free CBs -> OS_ERRNO_QUEUE_CB_UNAVAILABLE */
    setup_empty_cb_array(2);
    g_allQueue[0].queueState = OS_QUEUE_USED;
    g_allQueue[1].queueState = OS_QUEUE_USED;
    ret = OsQueueCreate(1, 4, &qId);
    assert_eq_u32("no free CB", OS_ERRNO_QUEUE_CB_UNAVAILABLE, ret);

    /* Case 2: free exists but allocation fails */
    setup_empty_cb_array(1);
    g_mock_alloc_fail = 1;
    ret = OsQueueCreate(2, 4, &qId);
    assert_eq_u32("allocation fail", OS_ERRNO_QUEUE_CREATE_NO_MEMORY, ret);

    /* Case 3: success initializes fields and nodes */
    setup_empty_cb_array(3);
    ret = OsQueueCreate(3, 5, &qId);
    assert_eq_u32("create ok", OS_OK, ret);
    /* first free index should be 0 */
    assert_true("qId==0", qId == 0);
    struct TagQueCb *cb0 = &g_allQueue[qId];
    U16 aligned = (U16)OsQueueGetNodeSize(5);
    assert_true("queue not NULL", cb0->queue != NULL);
    assert_true("nodeNum set", cb0->nodeNum == 3);
    assert_true("nodeSize aligned", cb0->nodeSize == aligned);
    assert_true("queueState used", cb0->queueState == OS_QUEUE_USED);
    assert_true("writableCnt == nodeNum", cb0->writableCnt == 3);
    assert_true("readableCnt == 0", cb0->readableCnt == 0);
    assert_true("head==0 tail==0", cb0->queueHead == 0 && cb0->queueTail == 0);
    assert_true("nodePeak==0", cb0->nodePeak == 0);
    /* verify node srcPid initialized */
    for (U16 i = 0; i < cb0->nodeNum; ++i)
    {
        struct QueNode *node = (struct QueNode *)(uintptr_t)&cb0->queue[i * cb0->nodeSize];
        if (node->srcPid != OS_QUEUE_PID_INVALID)
        {
            printf("[FAIL] node %u srcPid not invalid\n", i);
        }
        else
        {
            tests_passed++;
            tests_run++;
        }
    }

    /* Case 4: first element used, second free -> id should be 1 */
    setup_empty_cb_array(2);
    g_allQueue[0].queueState = OS_QUEUE_USED;
    ret = OsQueueCreate(1, 4, &qId);
    assert_eq_u32("id==1 when first used", OS_OK, ret);
    assert_true("qId==1", qId == 1);

    /* Case 5: different node sizes alignment check */
    setup_empty_cb_array(1);
    ret = OsQueueCreate(2, 6, &qId); /* ALIGN(6,4)=8 +4=12 */
    assert_eq_u32("create ok size6", OS_OK, ret);
    assert_true("aligned size==12", g_allQueue[qId].nodeSize == 12);

    float passRate = tests_run ? (100.0f * tests_passed / tests_run) : 0.0f;
    printf("Pass-Rate: %.2f%% (passed %d / total %d)\n", passRate, tests_passed, tests_run);

    free_all_resources();
    return (tests_passed == tests_run) ? 0 : 1;
}

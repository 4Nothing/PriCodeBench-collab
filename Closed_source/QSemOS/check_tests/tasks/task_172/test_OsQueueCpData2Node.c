#include "test_support.h"
#include "func_under_test.c"

static void setup(struct TagQueCb *q, U8 *buf, U16 nodeSize, U16 nodeNum)
{
    init_queue_cb(q, buf, nodeSize, nodeNum);
}

int main(void)
{
    int pass = 0, total = 0;
    reset_task();

    struct TagQueCb q;
    U8 storage[3 * 32] = {0};
    setup(&q, storage, 32, 3);

    /* Case 1: normal prio writes at tail */
    total++;
    U8 data1[4] = {1, 2, 3, 4};
    OsQueueCpData2Node(OS_QUEUE_NORMAL, (uintptr_t)data1, 4, &q);
    struct QueNode *n0 = (struct QueNode *)(q.queue + 0 * q.nodeSize);
    int ok1 = (q.queueTail == 1) && (n0->size == 4) && (n0->buf[0] == 1) && (n0->srcPid == OsGetSrcPid());
    printf("[CASE] normal write tail\n  Expect: tail=1 size=4 srcPid=%u\n  Actual: tail=%u size=%u srcPid=%u\n", OsGetSrcPid(), q.queueTail, n0->size, n0->srcPid);
    pass += ok1;

    /* Case 2: urgent prio writes at head (before current head) */
    total++;
    U8 data2[2] = {9, 8};
    OsQueueCpData2Node(OS_QUEUE_URGENT, (uintptr_t)data2, 2, &q);
    int headIdx = (int)q.queueHead;
    struct QueNode *nh = (struct QueNode *)(q.queue + headIdx * q.nodeSize);
    int ok2 = (headIdx == (q.nodeNum - 1)) && (nh->size == 2) && (nh->buf[0] == 9);
    printf("[CASE] urgent write head\n  Expect: head=%u size=2\n  Actual: head=%u size=%u\n", q.nodeNum - 1, q.queueHead, nh->size);
    pass += ok2;

    /* Case 3: peak updated */
    total++;
    int ok3 = (q.nodePeak >= 1);
    printf("[CASE] peak updated\n  Expect: >=1\n  Actual: %u\n", q.nodePeak);
    pass += ok3;

    /* Case 4: tail wrap around */
    total++;
    U8 d3[1] = {0};
    q.queueTail = q.nodeNum - 1;
    OsQueueCpData2Node(OS_QUEUE_NORMAL, (uintptr_t)d3, 1, &q);
    pass += assert_eq_u32("tail wrap", 0, q.queueTail);

    /* Case 5: head wrap around on urgent */
    total++;
    q.queueHead = 0;
    OsQueueCpData2Node(OS_QUEUE_URGENT, (uintptr_t)d3, 1, &q);
    pass += assert_eq_u32("head wrap", q.nodeNum - 1, q.queueHead);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

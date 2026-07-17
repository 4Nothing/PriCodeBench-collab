#include "test_support.h"
#include "func_under_test.c"

static void prepare_queue_with_one_msg(struct TagQueCb *q, U8 *buf, U16 nodeSize, U16 nodeNum, const void *msg, U16 msgLen)
{
    init_queue_cb(q, buf, nodeSize, nodeNum);
    /* Manually enqueue one node at tail */
    struct QueNode *n = (struct QueNode *)(q->queue + q->queueTail * q->nodeSize);
    n->size = msgLen;
    n->srcPid = 0x1234;
    memcpy(n->buf, msg, msgLen);
    q->queueTail = 1;
    q->readableCnt = 1;
    q->writableCnt = nodeNum - 1;
}

int main(void)
{
    int pass = 0, total = 0;
    /* g_maxQueue is already initialized to MAX_TEST_QUEUES (4) */
    printf("DEBUG: g_maxQueue address = %p, value = %u\n", (void*)&g_maxQueue, (unsigned)g_maxQueue);
    reset_task();

    struct TagQueCb *q = &g_queue_cbs[0];
    U8 storage[2 * 32] = {0};

    /* Case 1: invalid id (innerId >= g_maxQueue) */
    total++;
    U32 len = 4;
    U8 out[8];
    U32 r1 = PRT_QueueRead(g_maxQueue + 1, out, &len, 0);
    pass += assert_eq_u32("QueueRead invalid id", OS_ERRNO_QUEUE_INVALID, r1);

    /* Init queue 0 first - needed for parameter validation tests */
    prepare_queue_with_one_msg(q, storage, 32, 2, "ABCD", 4);

    /* Case 2: null buffer (queue initialized) */
    total++;
    r1 = PRT_QueueRead(0, NULL, &len, 0);
    pass += assert_eq_u32("QueueRead null buf", OS_ERRNO_QUEUE_PTR_NULL, r1);

    /* Case 3: len==0 (queue initialized) */
    total++;
    len = 0;
    r1 = PRT_QueueRead(0, out, &len, 0);
    pass += assert_eq_u32("QueueRead size zero", OS_ERRNO_QUEUE_SIZE_ZERO, r1);

    /* Re-init queue for remaining tests */
    prepare_queue_with_one_msg(q, storage, 32, 2, "ABCD", 4);

    /* Case 4: queue not created flag -> mark unused */
    total++;
    q->queueState = OS_QUEUE_UNUSED;
    len = 4;
    U32 r2 = PRT_QueueRead(0, out, &len, 0);
    pass += assert_eq_u32("QueueRead not create", OS_ERRNO_QUEUE_NOT_CREATE, r2);

    /* Case 5: success read with smaller buffer size truncation */
    total++;
    q->queueState = OS_QUEUE_USED;
    len = 2;
    U32 r3 = PRT_QueueRead(0, out, &len, 0);
    int ok3 = (r3 == OS_OK) && (len == 2) && (out[0] == 'A' && out[1] == 'B');
    printf("[CASE] QueueRead success truncate\n  Expect: ret=%u len=2 data=AB\n  Actual: ret=%u len=%u data=%c%c\n", OS_OK, r3, len, out[0], out[1]);
    pass += ok3;

    total++;
    len = 4;
    U32 r4 = PRT_QueueRead(0, out, &len, OS_QUEUE_NO_WAIT);
    pass += assert_eq_u32("QueueRead no data no wait", OS_ERRNO_QUEUE_NO_SOURCE, r4);

    /* Case 7: timeout path when empty and wait with finite timeout */
    total++;
    len = 1;
    U32 r5 = PRT_QueueRead(0, out, &len, 5);
    pass += assert_eq_u32("QueueRead timeout", OS_ERRNO_QUEUE_TIMEOUT, r5);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

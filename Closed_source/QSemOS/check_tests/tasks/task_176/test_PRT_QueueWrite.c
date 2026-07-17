#include "test_support.h"
#include "func_under_test.c"

static void setup_queue(struct TagQueCb *q, U8 *buf, U16 nodeSize, U16 nodeNum)
{
    init_queue_cb(q, buf, nodeSize, nodeNum);
}

int main(void)
{
    int pass = 0, total = 0;
    reset_task();
    struct TagQueCb *q = &g_queue_cbs[0];
    U8 storage[2 * 32] = {0};

    /* Case 1: invalid id in para check (innerId >= g_maxQueue) */
    total++;
    U8 data = 0xAA;
    U32 r1 = PRT_QueueWrite(g_maxQueue, &data, 1, 0, OS_QUEUE_NORMAL);
    pass += assert_eq_u32("QueueWrite invalid id", OS_ERRNO_QUEUE_INVALID, r1);

    /* Case 2: null buf */
    total++;
    r1 = PRT_QueueWrite(0, NULL, 1, 0, OS_QUEUE_NORMAL);
    pass += assert_eq_u32("QueueWrite null buf", OS_ERRNO_QUEUE_PTR_NULL, r1);

    /* Case 3: size zero */
    total++;
    r1 = PRT_QueueWrite(0, &data, 0, 0, OS_QUEUE_NORMAL);
    pass += assert_eq_u32("QueueWrite size zero", OS_ERRNO_QUEUE_SIZE_ZERO, r1);

    /* Case 4: prio invalid */
    total++;
    r1 = PRT_QueueWrite(0, &data, 1, 0, 99);
    pass += assert_eq_u32("QueueWrite prio invalid", OS_ERRNO_QUEUE_PRIO_INVALID, r1);

    /* Init queue, but mark unused */
    setup_queue(q, storage, 32, 2);
    q->queueState = OS_QUEUE_UNUSED;

    /* Case 5: not created */
    total++;
    r1 = PRT_QueueWrite(0, &data, 1, 0, OS_QUEUE_NORMAL);
    pass += assert_eq_u32("QueueWrite not create", OS_ERRNO_QUEUE_NOT_CREATE, r1);

    /* Case 6: size too big */
    total++;
    q->queueState = OS_QUEUE_USED;
    U8 big[40];
    r1 = PRT_QueueWrite(0, big, sizeof(big), 0, OS_QUEUE_NORMAL);
    pass += assert_eq_u32("QueueWrite too big", OS_ERRNO_QUEUE_SIZE_TOO_BIG, r1);

    /* Case 7: pend no wait when full returns NO_SOURCE */
    total++;
    /* Make queue full: writableCnt=0 */
    q->writableCnt = 0;
    r1 = PRT_QueueWrite(0, &data, 1, OS_QUEUE_NO_WAIT, OS_QUEUE_NORMAL);
    pass += assert_eq_u32("QueueWrite full nowait", OS_ERRNO_QUEUE_NO_SOURCE, r1);

    /* Case 8: timeout path when full */
    total++;
    r1 = PRT_QueueWrite(0, &data, 1, 5, OS_QUEUE_NORMAL);
    pass += assert_eq_u32("QueueWrite timeout", OS_ERRNO_QUEUE_TIMEOUT, r1);

    /* Case 9: success write increases readableCnt */
    total++;
    q->writableCnt = q->nodeNum;
    q->readableCnt = 0;
    q->queueHead = q->queueTail = 0;
    r1 = PRT_QueueWrite(0, &data, 1, 0, OS_QUEUE_NORMAL);
    int ok9 = (r1 == OS_OK) && (q->readableCnt == 1);
    printf("[CASE] QueueWrite success\n  Expect: ret=%u readableCnt=1\n  Actual: ret=%u readableCnt=%u\n", OS_OK, r1, q->readableCnt);
    pass += ok9;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include "common.h"
#include <stdio.h>

static void init_queue_cb(struct TagQueCb *q, U8 *buf, U16 nodeSize, U16 nodeNum)
{
    q->queue = buf;
    q->nodeSize = nodeSize;
    q->nodeNum = nodeNum;
    q->queueHead = 0;
    q->queueTail = 0;
    q->nodePeak = 0;
    q->readableCnt = 0;
    q->writableCnt = nodeNum;
    q->queueState = OS_QUEUE_USED;
    ListInit(&q->readList);
    ListInit(&q->writeList);
}

static int assert_eq_u32(const char *name, U32 expect, U32 actual)
{
    printf("[CASE] %s\n  Expect: %u\n  Actual: %u\n", name, expect, actual);
    return expect == actual;
}

static void print_pass_rate(int pass, int total)
{
    double rate = total ? (100.0 * pass / total) : 0.0;
    printf("Pass-Rate: %.2f%%\n", rate);
}

#endif /* TEST_SUPPORT_H */

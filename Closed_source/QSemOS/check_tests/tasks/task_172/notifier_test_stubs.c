/*
 * Stub implementations for queue tests
 * Contains all definitions that would otherwise be duplicated
 */
#include "test_support.h"

/* Global state definitions */
volatile int g_os_int_active = 0;
volatile int g_os_hwi_active = 0;
volatile U32 OS_TASK_LOCK_DATA = 0;

/* Task control block and running task */
static struct TagTskCb g_task0 = {.taskPid = 1, .taskStatus = 0};
struct TagTskCb *g_running_task = &g_task0;

/* Queue globals */
U16 g_maxQueue = MAX_TEST_QUEUES;
struct TagQueCb g_queue_cbs[MAX_TEST_QUEUES];
struct TagQueCb *g_allQueue = g_queue_cbs;

/* Task scheduler functions */
void OsTskReadyDel(struct TagTskCb *t) { (void)t; }
void OsTskReadyAddBgd(struct TagTskCb *t) { (void)t; }
void OsTskTimerAdd(struct TagTskCb *t, U32 ticks) { (void)t; (void)ticks; }
void OsTskSchedule(void) { }
void OsIntRestore(uintptr_t save) { (void)save; }

/* reset_task implementation */
void reset_task(void)
{
    g_task0.taskPid = 1;
    g_task0.taskStatus = 0;
    ListInit(&g_task0.pendList);
    g_task0.taskPend = NULL;
    g_running_task = &g_task0;
    g_os_int_active = 0;
    g_os_hwi_active = 0;
    OS_TASK_LOCK_DATA = 0;
}

/* OsGetSrcPid stub - needed by prt_queue.c */
U32 OsGetSrcPid(void) {
    if (OS_HWI_ACTIVE) {
        return COMPOSE_PID(0x0U, OS_HWI_HANDLE);
    }
    return RUNNING_TASK->taskPid;
}

/* init_queue_cb - initialize queue control block for testing */
void init_queue_cb(struct TagQueCb *q, U8 *buf, U16 nodeSize, U16 nodeNum)
{
    memset(q, 0, sizeof(*q));
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
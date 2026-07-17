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

/* Inline functions from prt_queue.c that need external linkage */
U32 OsInnerPend(U16 *count, struct TagListObject *pendList, U32 timeOut, struct TagQueCb *queueCb)
{
    /* 判断是否需要阻塞 */
    if (*count > 0) {
        (*count)--;
        return OS_OK;
    }

    /* 阻塞任务 */
    if (timeOut == OS_QUEUE_NO_WAIT) {
        return OS_ERRNO_QUEUE_NO_SOURCE;
    }

    if (OS_INT_ACTIVE) {
        return OS_ERRNO_QUEUE_IN_INTERRUPT;
    }

    /* 如果锁任务的情况下 */
    if (OS_TASK_LOCK_DATA != 0) {
        return OS_ERRNO_QUEUE_PEND_IN_LOCK;
    }

    /* 直接返回超时，因为我们不支持真正的任务调度 */
    return OS_ERRNO_QUEUE_TIMEOUT;
}

bool OsQueuePendNeedProc(struct TagListObject *objectList)
{
    /* 不支持真正的任务唤醒 */
    return FALSE;
}

void OsQueueCpData2Node(U32 prio, uintptr_t bufferAddr, U32 bufferSize, struct TagQueCb *queueCb)
{
    /* 简化的实现：只是移动tail指针 */
    if (prio == (U32)OS_QUEUE_NORMAL) {
        queueCb->queueTail++;
        if (queueCb->queueTail == queueCb->nodeNum) {
            queueCb->queueTail = 0;
        }
    } else {
        /* 紧急消息 - 简化处理 */
        if (queueCb->queueHead == 0) {
            queueCb->queueHead = queueCb->nodeNum - 1;
        } else {
            queueCb->queueHead--;
        }
    }
}
/*
 * Stub implementations for queue tests
 * Contains all definitions that would otherwise be duplicated
 */
#include "test_support.h"
#include <stdlib.h>
#include <string.h>

/* Global state definitions */
volatile int g_os_int_active = 0;
volatile int g_os_hwi_active = 0;
volatile U32 OS_TASK_LOCK_DATA = 0;

/* Mock allocation failure flags */
int g_mock_alloc_fail = 0;
int g_mock_memset_fail = 0;

/* Task control block and running task */
static struct TagTskCb g_task0 = {.taskPid = 1, .taskStatus = 0};
struct TagTskCb *g_running_task = &g_task0;

/* Queue globals - define here for testing (test modifies these) */
U16 g_maxQueue = 0;
struct TagQueCb *g_allQueue = NULL;

/* Memory allocation stubs for tests */
void *OsMemAlloc(U32 mid, U32 flags, U32 size)
{
    (void)mid;
    (void)flags;
    if (g_mock_alloc_fail) {
        g_mock_alloc_fail = 0;
        return NULL;
    }
    return malloc(size);
}

void OsMemFree(void *ptr)
{
    if (ptr)
        free(ptr);
}

/* memset stub that can fail */
int memset_s(void *dest, size_t destsz, int ch, size_t count)
{
    if (dest == NULL) return -1;
    if (count > destsz) return -1;
    if (g_mock_memset_fail) {
        g_mock_memset_fail = 0;
        return -1;
    }
    memset(dest, ch, count);
    return 0;
}

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
    (void)pendList;
    (void)queueCb;
    if (*count > 0) {
        (*count)--;
        return OS_OK;
    }
    if (timeOut == OS_QUEUE_NO_WAIT) {
        return OS_ERRNO_QUEUE_NO_SOURCE;
    }
    if (OS_INT_ACTIVE) {
        return OS_ERRNO_QUEUE_IN_INTERRUPT;
    }
    if (OS_TASK_LOCK_DATA != 0) {
        return OS_ERRNO_QUEUE_PEND_IN_LOCK;
    }
    return OS_ERRNO_QUEUE_TIMEOUT;
}

bool OsQueuePendNeedProc(struct TagListObject *objectList)
{
    (void)objectList;
    return FALSE;
}

void OsQueueCpData2Node(U32 prio, uintptr_t bufferAddr, U32 bufferSize, struct TagQueCb *queueCb)
{
    (void)bufferAddr;
    (void)bufferSize;
    if (prio == (U32)OS_QUEUE_NORMAL) {
        queueCb->queueTail++;
        if (queueCb->queueTail == queueCb->nodeNum) {
            queueCb->queueTail = 0;
        }
    } else {
        if (queueCb->queueHead == 0) {
            queueCb->queueHead = queueCb->nodeNum - 1;
        } else {
            queueCb->queueHead--;
        }
    }
}

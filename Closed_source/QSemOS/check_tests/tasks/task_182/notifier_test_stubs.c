/*
 * Stub implementations for queue tests
 * Contains all definitions that would otherwise cause issues
 */
#include "test_support.h"
#include "mocks/prt_queue_external.h"
#include "mocks/prt_mem_external.h"
#include <stdlib.h>
#include <string.h>

/* Global state definitions */
volatile int g_os_int_active = 0;
volatile int g_os_hwi_active = 0;
volatile U32 OS_TASK_LOCK_DATA = 0;
int g_mock_alloc_fail = 0; /* mock control for OsMemAlloc */

/* Note: g_mock_alloc_fail and g_mock_memset_fail are defined as static in mocks */
/* Do not redefine them here - they will be provided by the mocks */

/* Task control block and running task */
static struct TagTskCb g_task0 = {.taskPid = 1, .taskStatus = 0};
struct TagTskCb *g_running_task = &g_task0;

/* Note: g_maxQueue and g_allQueue are defined in prt_queue_init.c */
/* Do not redefine them here */
U16 g_maxQueue;
struct TagQueCb *g_allQueue;

/* Task scheduler functions - needed by prt_queue.c */
void OsTskReadyDel(struct TagTskCb *t) { (void)t; }
void OsTskReadyAddBgd(struct TagTskCb *t) { (void)t; }
void OsTskTimerAdd(struct TagTskCb *t, U32 ticks) { (void)t; (void)ticks; }
void OsTskSchedule(void) { }

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

/* Queue parameter check and create functions */
U32 OsQueueCreatParaCheck(U16 nodeNum, U16 maxNodeSize, U32 *queueId)
{
    if (queueId == NULL) {
        return OS_ERRNO_QUEUE_CREAT_PTR_NULL;
    }
    if (nodeNum == 0 || maxNodeSize == 0) {
        return OS_ERRNO_QUEUE_PARA_ZERO;
    }
    if (maxNodeSize > 65534) {
        return OS_ERRNO_QUEUE_NSIZE_INVALID;
    }
    return OS_OK;
}

U32 OsQueueCreate(U16 nodeNum, U16 maxNodeSize, U32 *qId)
{
    if (g_maxQueue == 0 || g_allQueue == NULL) {
        return OS_ERRNO_QUEUE_CB_UNAVAILABLE;
    }
    if (g_mock_alloc_fail) {
        g_mock_alloc_fail = 0;
        return OS_ERRNO_QUEUE_CREATE_NO_MEMORY;
    }
    U32 id = *qId;
    if (id >= g_maxQueue) {
        return OS_ERRNO_QUEUE_CB_UNAVAILABLE;
    }
    g_allQueue[id].nodeNum = nodeNum;
    g_allQueue[id].nodeSize = maxNodeSize;
    g_allQueue[id].queue = (U8 *)malloc(nodeNum * maxNodeSize);
    if (g_allQueue[id].queue == NULL) {
        return OS_ERRNO_QUEUE_CREATE_NO_MEMORY;
    }
    g_allQueue[id].queueState = OS_QUEUE_USED;
    g_allQueue[id].queueHead = 0;
    g_allQueue[id].queueTail = 0;
    g_allQueue[id].writableCnt = nodeNum;
    g_allQueue[id].readableCnt = 0;
    return OS_OK;
}
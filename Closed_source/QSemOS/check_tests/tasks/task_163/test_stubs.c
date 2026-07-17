/* Use original module stubs for types and minimal env */
#include "os_stub.h"
#include "test_helpers.h"

TagTskCb g_dummyTask;
TagTskCb *RUNNING_TASK = &g_dummyTask;

/* Mutable interrupt/lock flags */
U32 g_int_active = 0;
U32 g_task_lock_data = 0;

/* Minimal pool for GET_SEM used by prt_sem_external.h */
TagSemCb g_semPool[8];

/* Extern from module to bound handle range */
U16 g_maxSem;

/* Instrumentation counters */
U32 g_ready_del_count = 0;
U32 g_ready_add_count = 0;
U32 g_ready_add_bgd_count = 0;
U32 g_schedule_count = 0;
U32 g_timer_add_count = 0;

/* Override weak inline stubs to count operations */
TagOsRunQue g_rq;
TagOsRunQue *OsSpinLockRunTaskRq(void) { return &g_rq; }
void OsSpinUnLockRunTaskRq(TagOsRunQue *q) { (void)q; }
void OsSpinLockTaskRq(TagTskCb *t) { (void)t; }
void OsSpinUnlockTaskRq(TagTskCb *t) { (void)t; }
void OsTskReadyDel(TagTskCb *t)
{
    (void)t;
    g_ready_del_count++;
}
void OsTskReadyAdd(TagTskCb *t)
{
    (void)t;
    g_ready_add_count++;
}
void OsTskReadyAddBgd(TagTskCb *t)
{
    (void)t;
    g_ready_add_bgd_count++;
}
void OsTskTimerAdd(TagTskCb *t, U32 to)
{
    (void)t;
    (void)to;
    g_timer_add_count++;
}
void OsTskSchedule(void) { g_schedule_count++; }
void OsTskScheduleFastPs(uintptr_t x)
{
    (void)x;
    g_schedule_count++;
}

/* PID -> TCB resolver */
static TagTskCb *g_tasks_map[256];
TagTskCb *test_get_tcb_handle(U32 pid)
{
    if (pid < 256 && g_tasks_map[pid] != NULL)
    {
        return g_tasks_map[pid];
    }
    return &g_dummyTask;
}

void test_register_task(TagTskCb *tcb)
{
    if (tcb != NULL && tcb->taskPid < 256)
    {
        g_tasks_map[tcb->taskPid] = tcb;
    }
}

void test_reset(void)
{
    g_ready_del_count = g_ready_add_count = g_ready_add_bgd_count = 0;
    g_schedule_count = g_timer_add_count = 0;
    g_int_active = 0;
    g_task_lock_data = 0;
    test_init_task(&g_dummyTask, 0, 10);
    /* reset resolver map */
    for (int i = 0; i < 256; ++i)
        g_tasks_map[i] = NULL;
    test_register_task(&g_dummyTask);
    for (int i = 0; i < 8; ++i)
    {
        test_init_sem(&g_semPool[i], 1, SEM_TYPE_COUNT, 0, 0);
    }
    g_maxSem = 8;
}

void test_init_task(TagTskCb *tcb, U32 pid, TskPrior priority)
{
    if (tcb == NULL)
        return;
    ListInit(&tcb->pendList);
    ListInit(&tcb->semBList);
    tcb->taskPid = pid;
    tcb->priority = priority;
    tcb->origPriority = priority;
    tcb->taskPend = NULL;
    tcb->status = 0;
    test_register_task(tcb);
}

void test_init_sem(TagSemCb *sem, U32 semStat, U32 semType, U32 semCount, U32 semMode)
{
    if (sem == NULL)
        return;
    ListInit(&sem->semList);
    ListInit(&sem->semBList);
    sem->semCount = semCount;
    sem->semType = semType;
    sem->semMode = semMode;
    sem->semStat = semStat;
    sem->semOwner = OS_INVALID_OWNER_ID;
    sem->recurCount = 0;
}

/* Stub implementations for inline functions in prt_sem.c */
bool OsSemPostIsInvalid(struct TagSemCb *semPosted)
{
    (void)semPosted;
    return 0;
}

void OsGetPendTskMaxPriority(TagTskCb *taskCb, U32 *maxPriority)
{
    TskPrior curMaxPrior = *maxPriority;
    TagSemCb *curSem = NULL;
    TagTskCb *pendTask = NULL;

    LIST_FOR_EACH(curSem, &taskCb->semBList, TagSemCb, semBList) {
        if (!ListEmpty(&curSem->semList)) {
            pendTask = GET_TCB_PEND(OS_LIST_FIRST(&(curSem->semList)));
            if (pendTask->priority < curMaxPrior) {
                curMaxPrior = pendTask->priority;
            }
        }
    }
    *maxPriority = curMaxPrior;
}

void OsResortSemList(TagTskCb *taskCb)
{
    (void)taskCb;
}

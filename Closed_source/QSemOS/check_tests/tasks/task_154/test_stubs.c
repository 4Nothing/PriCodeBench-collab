/* Use original module stubs for types and minimal env */
#include "os_stub.h"
#include "test_helpers.h"

struct TagTskCb g_dummyTask;
struct TagTskCb *RUNNING_TASK = &g_dummyTask;
U16 g_maxSem;

/* Mutable interrupt/lock flags */
U32 g_int_active = 0;
U32 g_task_lock_data = 0;

/* Minimal pool for GET_SEM used by prt_sem_external.h */
struct TagSemCb g_semPool[8];

/* Extern from module to bound handle range */
extern U16 g_maxSem;

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
void OsSpinLockTaskRq(struct TagTskCb *t) { (void)t; }
void OsSpinUnlockTaskRq(struct TagTskCb *t) { (void)t; }
void OsTskReadyDel(struct TagTskCb *t)
{
    (void)t;
    g_ready_del_count++;
}
void OsTskReadyAdd(struct TagTskCb *t)
{
    (void)t;
    g_ready_add_count++;
}
void OsTskReadyAddBgd(struct TagTskCb *t)
{
    (void)t;
    g_ready_add_bgd_count++;
}
void OsTskTimerAdd(struct TagTskCb *t, U32 to)
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
static struct TagTskCb *g_tasks_map[256];
struct TagTskCb *test_get_tcb_handle(U32 pid)
{
    if (pid < 256 && g_tasks_map[pid] != NULL)
    {
        return g_tasks_map[pid];
    }
    return &g_dummyTask;
}

void test_register_task(struct TagTskCb *tcb)
{
    if (tcb != NULL && tcb->taskPid < 256)
    {
        g_tasks_map[tcb->taskPid] = tcb;
    }
}

/* Stub implementations for inline functions in prt_sem.c */
U32 OsSemPostErrorCheck(struct TagSemCb *semPosted, U32 semHandle)
{
    (void)semHandle;
    if (semPosted->semStat == OS_SEM_UNUSED) {
        return OS_ERRNO_SEM_INVALID;
    }
    if (GET_SEM_TYPE(semPosted->semType) == SEM_TYPE_COUNT) {
        if (semPosted->semCount >= OS_SEM_COUNT_MAX) {
            return OS_ERRNO_SEM_OVERFLOW;
        }
    }
    return OS_OK;
}

bool OsSemPostIsInvalid(struct TagSemCb *semPosted)
{
    (void)semPosted;
    return 0;
}

void OsGetPendTskMaxPriority(struct TagTskCb *taskCb, U32 *maxPriority)
{
    TskPrior curMaxPrior = *maxPriority;
    struct TagSemCb *curSem = NULL;
    struct TagTskCb *pendTask = NULL;

    LIST_FOR_EACH(curSem, &taskCb->semBList, struct TagSemCb, semBList) {
        if (!ListEmpty(&curSem->semList)) {
            pendTask = GET_TCB_PEND(OS_LIST_FIRST(&(curSem->semList)));
            if (pendTask->priority < curMaxPrior) {
                curMaxPrior = pendTask->priority;
            }
        }
    }
    *maxPriority = curMaxPrior;
}

void OsResortSemList(struct TagTskCb *taskCb)
{
    (void)taskCb;
}

void test_init_task(struct TagTskCb *tcb, U32 pid, U32 prio)
{
    memset(tcb, 0, sizeof(struct TagTskCb));
    tcb->taskPid = pid;
    tcb->priority = prio;
    tcb->origPriority = prio;
    ListInit(&tcb->pendList);
    ListInit(&tcb->semBList);
}

void test_init_sem(struct TagSemCb *sem, U32 stat, U32 type, U32 count, U32 mode)
{
    memset(sem, 0, sizeof(struct TagSemCb));
    sem->semStat = stat;
    sem->semType = type;
    sem->semCount = count;
    sem->semMode = mode;
    ListInit(&sem->semList);
    ListInit(&sem->semBList);
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

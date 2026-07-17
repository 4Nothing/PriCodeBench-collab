#include <stdio.h>
#include <string.h>
#include "test_support.h"
#include "prt_hook_external.h"

/* Globals expected by the production code */
struct TagTskCb *RUNNING_TASK = NULL;
struct TagTskCb *g_highestTask = NULL;
volatile uint32_t UNI_FLAG = 0;
volatile uint32_t g_uniTaskLock = 0;

/* Tick related */
void (*g_tickDispatcher)(void) = NULL;
volatile uint32_t TICK_NO_RESPOND_CNT = 0;

/* Fake time source */
U64 g_fakeTime = 0;

/* Observability */
uintptr_t g_lastContextLoadPtr = 0;
uint32_t g_contextLoadCount = 0;
uint32_t g_switchHookCount = 0;
uint32_t g_lastPrevPid = 0;
uint32_t g_lastNextPid = 0;
uint32_t g_irqRecordCount = 0;
uint32_t g_intEnableCount = 0;
uint32_t g_intDisableCount = 0;

/* Sample tasks */
struct TagTskCb g_taskA;
struct TagTskCb g_taskB;

/* Stubs */
void OsTskReadyDel(struct TagTskCb *tsk) { (void)tsk; }
void OsTskReadyAdd(struct TagTskCb *tsk) { (void)tsk; }
void OsTskContextLoad(uintptr_t tcbPtr)
{
    g_lastContextLoadPtr = tcbPtr;
    g_contextLoadCount++;
}
void OsTskHighestSet(void) { /* choose highest as B by default */ g_highestTask = &g_taskB; }
void OsTimeSliceUpdate(struct TagTskCb *tsk, U64 curr, uint32_t discount)
{
    if (tsk == NULL)
        return;
    /* simple behavior: if policy RR and timeSlice > 0, decrement by 1 + discount>0 */
    if (tsk->policy == OS_TSK_SCHED_RR && tsk->timeSlice > 0)
    {
        if (discount > 0 && tsk->timeSlice > discount)
            tsk->timeSlice -= discount;
        else if (discount > 0)
            tsk->timeSlice = 0;
        if (tsk->timeSlice > 0)
            tsk->timeSlice--;
    }
    (void)curr;
}

void OsTskSwitchHookCaller(uint32_t prevPid, uint32_t nextPid)
{
    g_switchHookCount++;
    g_lastPrevPid = prevPid;
    g_lastNextPid = nextPid;
}

U64 OsCurCycleGet64(void) { return g_fakeTime; }
void OsIntEnable(void) { g_intEnableCount++; }
void OsIntDisable(void) { g_intDisableCount++; }
void OS_IRQ_TIME_RECORD(U64 start)
{
    (void)start;
    g_irqRecordCount++;
}

void test_tick_dispatcher_noop(void) { /* no-op */ }

void init_task(struct TagTskCb *tsk, uint32_t pid, uint32_t policy, uint32_t timeSlice)
{
    memset(tsk, 0, sizeof(*tsk));
    tsk->taskPid = pid;
    tsk->policy = policy;
    tsk->timeSlice = timeSlice;
}

void reset_test_state(void)
{
    UNI_FLAG = 0;
    g_uniTaskLock = 0;
    RUNNING_TASK = NULL;
    g_highestTask = NULL;

    g_fakeTime = 0;

    g_lastContextLoadPtr = 0;
    g_contextLoadCount = 0;
    g_switchHookCount = 0;
    g_lastPrevPid = 0;
    g_lastNextPid = 0;
    g_irqRecordCount = 0;
    g_intEnableCount = 0;
    g_intDisableCount = 0;

    init_task(&g_taskA, 1, 0, 0);
    init_task(&g_taskB, 2, OS_TSK_SCHED_RR, 3);

    g_tickDispatcher = test_tick_dispatcher_noop;
    TICK_NO_RESPOND_CNT = 0;
}

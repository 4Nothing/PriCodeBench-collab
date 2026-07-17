#include <stdio.h>
#include <string.h>
#include "test_support.h"
#include "prt_hook_external.h"

/* Observability */
uintptr_t g_lastContextLoadPtr = 0;
uint32_t g_contextLoadCount = 0;
uint32_t g_switchHookCount = 0;
uint32_t g_lastPrevPid = 0;
uint32_t g_lastNextPid = 0;
uint32_t g_irqRecordCount = 0;
uint32_t g_intEnableCount = 0;
uint32_t g_intDisableCount = 0;

/* Fake time source - override the weak one in auto_stub.h */
U64 g_fakeTime = 0;
U64 OsCurCycleGet64(void) { return g_fakeTime; }

/* Interrupt enable/disable counters - use weak aliases */
void OsIntEnable(void) { g_intEnableCount++; }
void OsIntDisable(void) { g_intDisableCount++; }

/* IRQ time record - stub implementation */
void OsIrqTimeRecordShim(U64 start)
{
    (void)start;
    g_irqRecordCount++;
}

/* Context load hook */
void OsTskContextLoad(uintptr_t tcbPtr)
{
    g_lastContextLoadPtr = tcbPtr;
    g_contextLoadCount++;
}

/* Switch hook caller */
void OsTskSwitchHookCaller(uint32_t prevPid, uint32_t nextPid)
{
    g_switchHookCount++;
    g_lastPrevPid = prevPid;
    g_lastNextPid = nextPid;
}

/* Highest task setter */
void OsTskHighestSet(void) { /* choose highest as B by default */ g_highestTask = &g_taskB; }

/* Task instances used by tests */
struct TagTskCb g_taskA;
struct TagTskCb g_taskB;
struct TagTskCb *g_highestTask = NULL;

/* Time slice update - only handles RR scheduling */
void OsTimeSliceUpdate(struct TagTskCb *tsk, U64 curr, uint32_t discount)
{
    if (tsk == NULL)
        return;
    /* only if policy RR and timeSlice > 0, decrement */
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

/* Test tick dispatcher */
void test_tick_dispatcher_noop(void) { /* no-op */ }
void (*g_tickDispatcher)(void) = NULL;

/* Initialize a task control block */
void init_task(struct TagTskCb *tsk, uint32_t pid, uint32_t policy, uint32_t timeSlice)
{
    memset(tsk, 0, sizeof(*tsk));
    tsk->taskPid = pid;
    tsk->policy = policy;
    tsk->timeSlice = timeSlice;
}

/* Reset all global/testing state */
void reset_test_state(void)
{
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
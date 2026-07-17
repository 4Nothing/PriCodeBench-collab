#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include "prt_timer.h"
#include "prt_perf_pmu.h"

/* g_perfTimed - declared as extern in auto_stub.h, needs strong definition here */
SwPmu g_perfTimed = {{0}};

/* Test counters */
U32 g_update_count_calls = 0;
U32 g_overflow_calls = 0;

/* Stub control variables */
U32 g_stub_timer_create_ret = OS_OK;
U32 g_stub_timer_start_ret = OS_OK;
U32 g_stub_timer_delete_ret = OS_OK;
U32 g_stub_next_timer_id = 1;
U32 g_stub_created_timer_id = 0;

/* set_core_id - needed by tests but not in auto_stub.h */
static U32 g_core_id = 0;
void set_core_id(U32 id)
{
    g_core_id = id;
}

U32 PRT_GetCoreID(void)
{
    return g_core_id;
}

/* OsPerfFetchIrqRegs - defined in auto_stub.h as weak, but we need a strong def */
void OsPerfFetchIrqRegs(PerfRegs *regs)
{
    if (regs)
        memset(regs, 0, sizeof(*regs));
}

/* OsPerfUpdateEventCount - NOT in auto_stub.h, needs to be here */
void OsPerfUpdateEventCount(Event *event, U32 inc)
{
    g_update_count_calls++;
    if (!event)
        return;
    /* Update count on current core */
    U32 cid = PRT_GetCoreID();
    event->count[cid] += inc;
}

/* OsPerfHandleOverFlow - NOT in auto_stub.h, needs to be here */
void OsPerfHandleOverFlow(Event *event, PerfRegs *regs)
{
    (void)event;
    (void)regs;
    g_overflow_calls++;
}

/* OsPerfPmuRegister - defined in auto_stub.h as weak, but we need a strong def */
U32 OsPerfPmuRegister(Pmu *pmu)
{
    /* Just accept */
    (void)pmu;
    return OS_OK;
}

/* OsPerfSwtimer - NOT in auto_stub.h, this is the main function being tested */
void OsPerfSwtimer(TimerHandle handle, U32 arg1, U32 arg2, U32 arg3, U32 arg4)
{
    (void)handle; (void)arg1; (void)arg2; (void)arg3; (void)arg4;
    /* Trigger update by calling OsPerfUpdateEventCount */
    Event *event = (Event *)&g_perfTimed.pmu.events.per[0];
    OsPerfUpdateEventCount(event, 1);
    OsPerfHandleOverFlow(event, NULL);
}

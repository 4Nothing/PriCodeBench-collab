#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include "prt_timer.h"
#include "prt_perf_pmu.h"

SwPmu g_perfTimed = {{0}};
U32 g_swtPmuId = 0;

U32 g_stub_timer_create_ret = OS_OK;
U32 g_stub_timer_start_ret = OS_OK;
U32 g_stub_timer_delete_ret = OS_OK;
U32 g_stub_next_timer_id = 1;
U32 g_stub_created_timer_id = 0;
struct TimerCreatePara g_last_timer_para = {0};

U32 g_primaryCoreId = 0;
static U32 g_core_id = 0;

U32 g_update_count_calls = 0;
U32 g_overflow_calls = 0;

U32 PRT_TimerCreate(const struct TimerCreatePara *para, U32 *id)
{
    if (para)
    {
        g_last_timer_para = *para;
    }
    *id = g_stub_next_timer_id++;
    g_stub_created_timer_id = *id;
    return g_stub_timer_create_ret;
}

U32 PRT_TimerStart(U32 grp, U32 id)
{
    (void)grp;
    (void)id;
    return g_stub_timer_start_ret;
}

U32 PRT_TimerDelete(U32 grp, U32 id)
{
    (void)grp;
    (void)id;
    return g_stub_timer_delete_ret;
}

U32 PRT_GetCoreID(void)
{
    return g_core_id;
}

void set_core_id(U32 id)
{
    g_core_id = id;
}

void PRT_Printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

void OsPerfFetchIrqRegs(PerfRegs *regs)
{
    if (regs)
        memset(regs, 0, sizeof(*regs));
}

void OsPerfUpdateEventCount(Event *event, U32 inc)
{
    g_update_count_calls++;
    if (!event)
        return;
    U32 cid = PRT_GetCoreID();
    event->count[cid] += inc;
}

void OsPerfHandleOverFlow(Event *event, PerfRegs *regs)
{
    (void)event;
    (void)regs;
    g_overflow_calls++;
}

U32 OsPerfPmuRegister(Pmu *pmu)
{
    return OS_OK;
}

void OsPerfSwtimer(TimerHandle handle, U32 arg1, U32 arg2, U32 arg3, U32 arg4)
{
    (void)handle; (void)arg1; (void)arg2; (void)arg3; (void)arg4;
}

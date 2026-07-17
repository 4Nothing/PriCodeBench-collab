/* Stub definitions for task_109 tests - minimal common set */
#include "mock_include/prt_perf.h"
#include "prt_perf_output.h"
#include <stdarg.h>

struct TagTskCb g_runningTask = {0};

/* OsPerfSetIrqRegs stub */
void OsPerfSetIrqRegs(uintptr_t pc, uintptr_t fp) {
    g_runningTask.pc = pc;
    g_runningTask.fp = fp;
}

/* Perf global control block */
PerfCB g_perfCb = {0};

/* Perf output stubs - weak to allow override */
__attribute__((weak)) void OsPerfOutPutFlush(void) { }

/* Runtime APIs */
U64 OsCurCycleGet64(void) { return 0; }
U64 PRT_ClkGetCycleCount64(void) { return 0; }
U32 PRT_GetCoreID(void) { return 0; }
U32 PRT_TaskSelf(TskHandle *threadId) { (void)threadId; return OS_OK; }

/* Perf functions - defined here but may be redefined by test files */
__attribute__((weak)) U32 PRT_PerfInit(void *buf, U32 size) {
    (void)buf; (void)size;
    g_perfCb.status = PERF_STARTED;
    return OS_OK;
}

__attribute__((weak)) U32 PRT_PerfConfig(PerfConfigAttr *attr) {
    (void)attr;
    return OS_OK;
}

__attribute__((weak)) void PRT_PerfStart(U32 sectionId) {
    (void)sectionId;
    g_perfCb.status = PERF_STARTED;
    g_perfCb.startTime = 100;
}

__attribute__((weak)) void PRT_PerfStop(void) {
    OsPerfOutPutFlush();
    g_perfCb.status = PERF_STOPED;
    g_perfCb.endTime = 200;
}

__attribute__((weak)) U32 PRT_PerfDataRead(char *dest, U32 size) {
    (void)dest; (void)size;
    return 0;
}

__attribute__((weak)) void PRT_PerfNotifyHookReg(PERF_BUF_NOTIFY_HOOK func) { (void)func; }
__attribute__((weak)) void PRT_PerfFlushHookReg(PERF_BUF_FLUSH_HOOK func) { (void)func; }

/* Internal perf functions */
__attribute__((weak)) void OsPerfStart(void) { g_perfCb.status = PERF_STARTED; }
__attribute__((weak)) void OsPerfStop(void) { g_perfCb.status = PERF_STOPED; }
__attribute__((weak)) void OsPerfPrintCount(void) { }
__attribute__((weak)) U32 OsPerfCollectData(Event *event, PerfSampleData *data, PerfRegs *regs) {
    (void)event; (void)data; (void)regs;
    return 0;
}
__attribute__((weak)) int OsPerfTaskFilter(U32 taskId) { (void)taskId; return 0; }
__attribute__((weak)) bool OsPerfParamValid(void) { return true; }
__attribute__((weak)) void OsPerfUpdateEventCount(Event *event, U32 value) { (void)event; (void)value; }
__attribute__((weak)) void OsPerfHandleOverFlow(Event *event, PerfRegs *regs) { (void)event; (void)regs; }
__attribute__((weak)) U32 OsPerfHdrInit(U32 id) { (void)id; return OS_OK; }
__attribute__((weak)) void OsPerfSwtimer(U32 handle, U32 arg1, U32 arg2, U32 arg3, U32 arg4) {
    (void)handle; (void)arg1; (void)arg2; (void)arg3; (void)arg4;
}

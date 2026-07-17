#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "mock_include/prt_perf.h"
#include "mock_include/prt_perf_pmu.h"
#include "mock_include/prt_perf_output.h"

struct TagTskCb g_runningTask = {.taskPid = 1, .pc = 0, .fp = 0};
PerfCB g_perfCb = {0};
Pmu *g_pmu = NULL;
U32 g_systemClock = 1000000;
U32 g_primaryCoreId = 0;
TickModInfo g_tickModInfo = {.tickPerSecond = 1000};

U64 OsCurCycleGet64(void) { return 1000; }
U64 PRT_ClkGetCycleCount64(void) { return 1000; }
U32 PRT_GetCoreID(void) { return 0; }
U32 PRT_TaskSelf(TskHandle *threadId) { *threadId = 1; return OS_OK; }
U32 memcpy_s(void *dest, size_t destMax, const void *src, size_t count) {
    if (destMax < count) return OS_ERROR;
    memcpy(dest, src, count);
    return OS_OK;
}
U32 memset_s(void *dest, size_t destMax, int c, size_t count) {
    if (destMax < count) return OS_ERROR;
    memset(dest, c, count);
    return OS_OK;
}
int PRT_Printf(const char *fmt, ...) { (void)fmt; return 0; }

SplLock g_perfSpin = 0;
U32 OsIntLock(void) { return 0; }
void OsIntRestore(U32 intSave) { (void)intSave; }
U32 PRT_SplLockInit(SplLock *lock) { (void)lock; return OS_OK; }
#ifndef SKIP_OsPmuInit
U32 OsPmuInit(void) { return OS_OK; }
#endif
#ifndef SKIP_OsHwPmuInit
U32 OsHwPmuInit(void) { return OS_OK; }
#endif
#ifndef SKIP_OsTimedPmuInit
U32 OsTimedPmuInit(void) { return OS_OK; }
#endif
#ifndef SKIP_OsSwPmuInit
U32 OsSwPmuInit(void) { return OS_OK; }
#endif

#ifndef SKIP_OsPerfOutPutInit
U32 OsPerfOutPutInit(void *buf, U32 size) { (void)buf; (void)size; return OS_OK; }
#endif
#ifndef SKIP_OsPerfOutPutWrite
U32 OsPerfOutPutWrite(const char *data, U32 len) { (void)data; (void)len; return OS_OK; }
#endif
#ifndef SKIP_OsPerfOutPutRead
U32 OsPerfOutPutRead(char *dest, U32 size) { (void)dest; (void)size; return 0; }
#endif
#ifndef SKIP_OsPerfOutPutFlush
void OsPerfOutPutFlush(void) { }
#endif
void OsPerfNotifyHookReg(PERF_BUF_NOTIFY_HOOK func) { (void)func; }
void OsPerfFlushHookReg(PERF_BUF_FLUSH_HOOK func) { (void)func; }

#ifndef SKIP_OsPerfSetIrqRegs
void OsPerfSetIrqRegs(uintptr_t pc, uintptr_t fp) { RUNNING_TASK->pc = pc; RUNNING_TASK->fp = fp; }
#endif

#ifndef SKIP_OsPerfUpdateEventCount
void OsPerfUpdateEventCount(Event *event, U32 value) {
    if (event == NULL) return;
    event->count[PRT_GetCoreID()] += (value & 0xFFFFFFFF);
}
#endif

#ifndef SKIP_OsPerfTaskFilter
int OsPerfTaskFilter(U32 taskId) {
    if (g_perfCb.taskIdsNr == 0) return TRUE;
    for (U32 i = 0; i < g_perfCb.taskIdsNr; i++)
        if (g_perfCb.taskIds[i] == taskId) return TRUE;
    return FALSE;
}
#endif

#ifndef SKIP_OsPerfParamValid
bool OsPerfParamValid(void) {
    if (g_pmu == NULL) return FALSE;
    U32 res = 0;
    for (U32 i = 0; i < g_pmu->events.nr; i++)
        res |= g_pmu->events.per[i].period;
    return (res != 0);
}
#endif

#ifndef SKIP_OsPerfHandleOverFlow
void OsPerfHandleOverFlow(Event *event, PerfRegs *regs) { (void)event; (void)regs; }
#endif

#ifndef SKIP_OsPerfCollectData
U32 OsPerfCollectData(Event *event, PerfSampleData *data, PerfRegs *regs) {
    U32 size = 0;
    char *p = (char *)data;
    if (g_perfCb.sampleType & PERF_RECORD_CPU) { *(U32 *)(p + size) = PRT_GetCoreID(); size += sizeof(data->cpuid); }
    if (g_perfCb.sampleType & PERF_RECORD_TID) { *(U32 *)(p + size) = 1; size += sizeof(data->taskId); }
    if (g_perfCb.sampleType & PERF_RECORD_TYPE) { *(U32 *)(p + size) = event->eventId; size += sizeof(data->eventId); }
    if (g_perfCb.sampleType & PERF_RECORD_PERIOD) { *(U32 *)(p + size) = event->period; size += sizeof(data->period); }
    if (g_perfCb.sampleType & PERF_RECORD_TIMESTAMP) { *(U64 *)(p + size) = PRT_ClkGetCycleCount64(); size += sizeof(data->time); }
    if (g_perfCb.sampleType & PERF_RECORD_IP) { *(uintptr_t *)(p + size) = regs->pc; size += sizeof(data->pc); }
    if (g_perfCb.sampleType & PERF_RECORD_CALLCHAIN) { *(U32 *)(p + size) = 0; size += sizeof(data->callChain.ipNr) + PERF_MAX_CALLCHAIN_DEPTH * sizeof(data->callChain.ip[0]); }
    return size;
}
#endif

#ifndef SKIP_OsPerfPmuGet
Pmu *OsPerfPmuGet(U32 type) { (void)type; return NULL; }
#endif

#ifndef SKIP_OsPerfConfig
U32 OsPerfConfig(PerfEventConfig *eventsCfg) {
    if (eventsCfg == NULL) return OS_ERRNO_PERF_CONFIG_NULL;
    g_pmu = OsPerfPmuGet(eventsCfg->type);
    if (g_pmu == NULL) return OS_ERRNO_PERF_INVALID_PMU;
    U32 eventNum = eventsCfg->eventsNr;
    if (eventNum > PERF_MAX_EVENT) eventNum = PERF_MAX_EVENT;
    memset(&g_pmu->events, 0, sizeof(PerfEvent));
    for (U32 i = 0; i < eventNum; i++) {
        g_pmu->events.per[i].eventId = eventsCfg->events[i].eventId;
        g_pmu->events.per[i].period = eventsCfg->events[i].period;
    }
    g_pmu->events.nr = eventNum;
    g_pmu->events.cntDivided = eventsCfg->predivided;
    g_pmu->type = eventsCfg->type;
    if (g_pmu->config) {
        U32 ret = g_pmu->config();
        if (ret != OS_OK) { memset(&g_pmu->events, 0, sizeof(PerfEvent)); return OS_ERRNO_PERF_PMU_CONFIG_ERROR; }
    }
    return OS_OK;
}
#endif

#ifndef SKIP_OsPerfStart
void OsPerfStart(void) {
    if (g_pmu == NULL) return;
    U32 cpuid = PRT_GetCoreID();
    if (g_perfCb.pmuStatusPerCpu[cpuid] != PERF_PMU_STARTED) {
        if (g_pmu->start) g_pmu->start();
        g_perfCb.pmuStatusPerCpu[cpuid] = PERF_PMU_STARTED;
    }
}
#endif

#ifndef SKIP_OsPerfStop
void OsPerfStop(void) {
    if (g_pmu == NULL) return;
    U32 cpuid = PRT_GetCoreID();
    if (g_perfCb.pmuStatusPerCpu[cpuid] != PERF_PMU_STOPED) {
        if (g_pmu->stop) g_pmu->stop();
        if (!g_perfCb.needStoreToBuffer) OsPerfPrintCount();
        g_perfCb.pmuStatusPerCpu[cpuid] = PERF_PMU_STOPED;
    }
}
#endif

#ifndef SKIP_OsPerfPrintCount
void OsPerfPrintCount(void) {
    if (g_pmu == NULL) return;
    for (U32 i = 0; i < g_pmu->events.nr; i++) {
        if (g_pmu->events.per[i].period == 0) continue;
    }
}
#endif

U32 OsPerfHdrInit(U32 id) {
    PerfDataHdr head = { .magic = PERF_DATA_MAGIC_WORD, .sampleType = g_perfCb.sampleType, .sectionId = id, .eventType = g_pmu ? g_pmu->type : 0, .len = sizeof(PerfDataHdr) };
    return OsPerfOutPutWrite((char *)&head, head.len);
}

#ifndef SKIP_PRT_PerfInit
U32 PRT_PerfInit(void *buf, U32 size) {
    if (g_perfCb.status != PERF_UNINIT) return OS_ERRNO_PERF_STATUS_INVALID;
    U32 ret = OsPerfOutPutInit(buf, size);
    if (ret != OS_OK) return OS_ERRNO_PERF_BUF_ERROR;
    g_perfCb.status = PERF_STOPED;
    return OS_OK;
}
#endif

#ifndef SKIP_PRT_PerfConfig
U32 PRT_PerfConfig(PerfConfigAttr *attr) {
    if (attr == NULL) return OS_ERRNO_PERF_CONFIG_NULL;
    if (g_perfCb.status != PERF_STOPED) return OS_ERRNO_PERF_STATUS_INVALID;
    g_pmu = NULL;
    g_perfCb.needStoreToBuffer = attr->needStoreToBuffer;
    g_perfCb.taskFilterEnable = attr->taskFilterEnable;
    g_perfCb.sampleType = attr->sampleType;
    if (attr->taskFilterEnable) {
        memcpy(g_perfCb.taskIds, attr->taskIds, attr->taskIdsNr * sizeof(U32));
        g_perfCb.taskIdsNr = attr->taskIdsNr;
    }
    return OsPerfConfig(&attr->eventsCfg);
}
#endif

#ifndef SKIP_PRT_PerfStart
void PRT_PerfStart(U32 sectionId) {
    if (g_perfCb.status != PERF_STOPED) return;
    if (!OsPerfParamValid()) return;
    if (g_perfCb.needStoreToBuffer) OsPerfHdrInit(sectionId);
    OsPerfStart();
    g_perfCb.status = PERF_STARTED;
    g_perfCb.startTime = OsCurCycleGet64();
}
#endif

#ifndef SKIP_PRT_PerfStop
void PRT_PerfStop(void) {
    if (g_perfCb.status != PERF_STARTED) return;
    OsPerfStop();
    OsPerfOutPutFlush();
    g_perfCb.status = PERF_STOPED;
    g_perfCb.endTime = OsCurCycleGet64();
}
#endif

#ifndef SKIP_PRT_PerfDataRead
U32 PRT_PerfDataRead(char *dest, U32 size) { return OsPerfOutPutRead(dest, size); }
#endif

#ifndef SKIP_PRT_PerfNotifyHookReg
void PRT_PerfNotifyHookReg(PERF_BUF_NOTIFY_HOOK func) { OsPerfNotifyHookReg(func); }
#endif

#ifndef SKIP_PRT_PerfFlushHookReg
void PRT_PerfFlushHookReg(PERF_BUF_FLUSH_HOOK func) { OsPerfFlushHookReg(func); }
#endif

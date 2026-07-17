/*
 * Wrapper file to include prt_perf.c functions without type conflicts
 * This file undefs all macros and types before including prt_perf.c
 */

// First undef all conflicting macros
#ifdef PRT_PERF_H
#undef PRT_PERF_H
#endif
#ifdef PRT_PERF_COMM_H
#undef PRT_PERF_COMM_H
#endif
#ifdef PRT_PMU_EXTERNAL_H
#undef PRT_PMU_EXTERNAL_H
#endif
#ifdef PRT_STACKTRACE_H
#undef PRT_STACKTRACE_H
#endif
#ifdef PRT_ATOMIC_H
#undef PRT_ATOMIC_H
#endif

// Undef all perf-related macros
#ifdef PERF_MAX_EVENT
#undef PERF_MAX_EVENT
#endif
#ifdef PERF_MAX_FILTER_TSKS
#undef PERF_MAX_FILTER_TSKS
#endif
#ifdef PERF_MAX_CALLCHAIN_DEPTH
#undef PERF_MAX_CALLCHAIN_DEPTH
#endif
#ifdef PERF_PMU_STOPED
#undef PERF_PMU_STOPED
#endif
#ifdef PERF_PMU_STARTED
#undef PERF_PMU_STARTED
#endif
#ifdef PERF_UNINIT
#undef PERF_UNINIT
#endif
#ifdef PERF_STARTED
#undef PERF_STARTED
#endif
#ifdef PERF_STOPED
#undef PERF_STOPED
#endif
#ifdef PERF_EVENT_TYPE_HW
#undef PERF_EVENT_TYPE_HW
#endif
#ifdef PERF_EVENT_TYPE_TIMED
#undef PERF_EVENT_TYPE_TIMED
#endif
#ifdef PERF_EVENT_TYPE_SW
#undef PERF_EVENT_TYPE_SW
#endif
#ifdef PERF_EVENT_TYPE_RAW
#undef PERF_EVENT_TYPE_RAW
#endif
#ifdef PERF_EVENT_TYPE_MAX
#undef PERF_EVENT_TYPE_MAX
#endif
#ifdef PERF_RECORD_CPU
#undef PERF_RECORD_CPU
#endif
#ifdef PERF_RECORD_TID
#undef PERF_RECORD_TID
#endif
#ifdef PERF_RECORD_TYPE
#undef PERF_RECORD_TYPE
#endif
#ifdef PERF_RECORD_PERIOD
#undef PERF_RECORD_PERIOD
#endif
#ifdef PERF_RECORD_TIMESTAMP
#undef PERF_RECORD_TIMESTAMP
#endif
#ifdef PERF_RECORD_IP
#undef PERF_RECORD_IP
#endif
#ifdef PERF_RECORD_CALLCHAIN
#undef PERF_RECORD_CALLCHAIN
#endif
#ifdef PERF_DATA_MAGIC_WORD
#undef PERF_DATA_MAGIC_WORD
#endif
#ifdef PERF_EVENT_TO_CODE
#undef PERF_EVENT_TO_CODE
#endif
#ifdef PERF_CODE_TO_EVENT
#undef PERF_CODE_TO_EVENT
#endif
#ifdef PRT_KERNEL_CORE_NUM
#undef PRT_KERNEL_CORE_NUM
#endif
#ifdef SMP_CALL_PERF_FUNC
#undef SMP_CALL_PERF_FUNC
#endif
#ifdef OS_SYS_US_PER_SECOND
#undef OS_SYS_US_PER_SECOND
#endif

// Undef all perf-related types
#ifdef PerfStatus
#undef PerfStatus
#endif
#ifdef PmuStatus
#undef PmuStatus
#endif
#ifdef PerfEventType
#undef PerfEventType
#endif
#ifdef PerfSampleType
#undef PerfSampleType
#endif
#ifdef PERF_BUF_NOTIFY_HOOK
#undef PERF_BUF_NOTIFY_HOOK
#endif
#ifdef PERF_BUF_FLUSH_HOOK
#undef PERF_BUF_FLUSH_HOOK
#endif
#ifdef PerfRegs
#undef PerfRegs
#endif
#ifdef PerfBackTrace
#undef PerfBackTrace
#endif
#ifdef PerfSampleData
#undef PerfSampleData
#endif
#ifdef PerfDataHdr
#undef PerfDataHdr
#endif
#ifdef Event
#undef Event
#endif
#ifdef PerfEvent
#undef PerfEvent
#endif
#ifdef Pmu
#undef Pmu
#endif
#ifdef PerfCB
#undef PerfCB
#endif
#ifdef PerfEventConfig
#undef PerfEventConfig
#endif
#ifdef PerfConfigAttr
#undef PerfConfigAttr
#endif
#ifdef TagTskCb
#undef TagTskCb
#endif
#ifdef TickModInfo
#undef TickModInfo
#endif

// Now include the mock header first to define our types
#include "../../mock_include/prt_perf.h"

// Now include the source code file
#include "../../../src/extended/perf/prt_perf.c"
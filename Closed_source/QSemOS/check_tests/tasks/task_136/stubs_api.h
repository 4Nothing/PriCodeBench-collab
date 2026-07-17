#pragma once
#include "prt_timer.h"
#include "perf/prt_perf_pmu.h"

/* Expose stub controls for tests */
extern U32 g_stub_timer_create_ret;
extern U32 g_stub_timer_start_ret;
extern U32 g_stub_timer_delete_ret;
extern U32 g_stub_next_timer_id;
extern U32 g_stub_created_timer_id;
extern struct TimerCreatePara g_last_timer_para;

extern U32 g_update_count_calls;
extern U32 g_overflow_calls;

void set_core_id(U32 id);
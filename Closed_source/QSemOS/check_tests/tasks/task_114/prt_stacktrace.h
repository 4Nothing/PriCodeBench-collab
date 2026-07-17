#pragma once
#include "prt_perf.h"

U32 PRT_GetStackTraceByTaskID(U32 *depth, uintptr_t *callChain, U32 taskId);

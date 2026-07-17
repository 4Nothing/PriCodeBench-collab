#pragma once
#include "auto_stub.h"

typedef U64 (*SysTimeHook)(void);
extern SysTimeHook g_sysTimeHook;

/* Stub for OsCurCycleGet64 - called by func_under_test.c's OsTimerDelayCount */
static inline U64 OsCurCycleGet64(void) {
    if (g_sysTimeHook != NULL) {
        return g_sysTimeHook();
    }
    return 0;
}

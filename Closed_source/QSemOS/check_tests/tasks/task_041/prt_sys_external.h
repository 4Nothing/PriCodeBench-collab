#pragma once
#include "auto_stub.h"

typedef U64 (*SysTimeHook)(void);
extern SysTimeHook g_sysTimeHook;

/* Stub for OsCurCycleGet64 - called by OsTimerDelayCount */
static inline U64 OsCurCycleGet64(void) {
    if (g_sysTimeHook != NULL) {
        return g_sysTimeHook();
    }
    return 0;
}

/* Stub for OsTimerDelayCount - called by func_under_test.c's PRT_ClkDelayUs */
static inline void OsTimerDelayCount(U64 cycles) {
    U64 cur;
    U64 end;
    cur = OsCurCycleGet64();
    end = cur + cycles;
    if (cur < end) {
        while (cur < end) {
            cur = OsCurCycleGet64();
        }
    } else {
        while (cur > end) {
            cur = OsCurCycleGet64();
        }
        while (cur < end) {
            cur = OsCurCycleGet64();
        }
    }
}

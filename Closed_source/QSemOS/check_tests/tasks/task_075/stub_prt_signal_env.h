#pragma once
#include "auto_stub.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Section and inline macros used in source
#define OS_SEC_L4_TEXT
#define OS_SEC_ALW_INLINE

// Signal helpers
    static inline int sigValid(int s) { return s >= 0 && s < (int)PRT_SIGNAL_MAX; }
    static inline signalSet sigMask(int s) { return (signalSet)1ull << (s & 63); }

// Global running task for tests (defined in auto_stub.h)
    void StubResetTask(void);

// TCB helpers
#define GET_TCB_HANDLE(id) (&g_runningTask)
#define CHECK_TSK_PID_OVERFLOW(id) (0)
#define TSK_IS_UNUSED(t) (0)

#ifdef __cplusplus
}
#endif
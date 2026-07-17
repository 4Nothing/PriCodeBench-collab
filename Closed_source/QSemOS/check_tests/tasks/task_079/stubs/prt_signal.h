#pragma once
#include "auto_stub.h"

// Section and inline macros used in source
#define OS_SEC_L4_TEXT
#define OS_SEC_ALW_INLINE
#define INLINE inline

// Signal-specific types and constants
// Use #ifndef to avoid conflicts with qsem_test_types.h and qsem_test_stubs.h

#ifndef PRT_SIGNAL_MAX
#define PRT_SIGNAL_MAX 64
#endif

#ifndef OS_SIGNAL_WAIT_FOREVER
#define OS_SIGNAL_WAIT_FOREVER 0xFFFFFFFFu
#endif

// Task status flags used by signal code
// These must match the values expected by prt_signal.c
#ifndef OS_TSK_TIMEOUT
#define OS_TSK_TIMEOUT 0x0020U
#endif
#ifndef OS_TSK_WAIT_SIGNAL
#define OS_TSK_WAIT_SIGNAL 0x0040U
#endif
#ifndef OS_TSK_READY
#define OS_TSK_READY 0x0004U
#endif
#ifndef OS_TSK_DELAY
#define OS_TSK_DELAY 0x0020U
#endif
#ifndef OS_TSK_SIG_PAUSE
#define OS_TSK_SIG_PAUSE 0x0080U
#endif
#ifndef OS_TSK_DELAY_INTERRUPTIBLE
#define OS_TSK_DELAY_INTERRUPTIBLE 0x0400U
#endif
#ifndef OS_TSK_HOLD_SIGNAL
#define OS_TSK_HOLD_SIGNAL 0x1000U
#endif
#ifndef OS_TSK_SUSPEND
#define OS_TSK_SUSPEND 0x0010U
#endif

#ifndef TSK_STATUS_SET
#define TSK_STATUS_SET(t, f) ((t)->taskStatus |= (f))
#endif
#ifndef TSK_STATUS_CLEAR
#define TSK_STATUS_CLEAR(t, f) ((t)->taskStatus &= ~(f))
#endif

// Signal set type - must be U64 for prt_signal.c to work correctly
// Override the qsem_test_types.h definition which uses uint32_t
#ifndef OS_SIGNAL_SET_DEFINED
#define OS_SIGNAL_SET_DEFINED
typedef U64 signalSet;
#endif

// Signal helpers - use #ifndef to avoid conflicts
#ifndef OS_SIG_VALID_DEFINED
#define OS_SIG_VALID_DEFINED
static inline bool sigValid(int s) { return s >= 0 && s < (int)PRT_SIGNAL_MAX; }
#endif

// sigMask as a function for 64-bit signalSet
#ifndef OS_SIG_MASK_DEFINED
#define OS_SIG_MASK_DEFINED
extern inline signalSet sigMaskFunc(int s) { return (signalSet)1ULL << (s & 63); }
#define sigMask(signo) sigMaskFunc(signo)
#endif

// Define OS_SIG_DEFAULT_HANDLER_DEFINED to prevent NULL definition in qsem_test_types.h
// This allows the real OsSigDefaultHandler function in prt_signal.c to be used
#define OS_SIG_DEFAULT_HANDLER_DEFINED

// StubResetTask function declaration
extern void StubResetTask(void);

// Force inline functions from prt_signal.c to be exported
// These are declared as inline in prt_signal.c but need extern declaration for linking
extern void OsSignalTimeOutSet(struct TagTskCb *runTsk, U32 timeOutTick);
extern U32 OsSignalWaitSche(struct TagTskCb *runTsk, const signalSet *set, U32 timeOutTick);
extern U32 OsSigWaitProcPendingSignal(struct TagTskCb *runTsk, const signalSet *set, signalInfo *info);
extern U32 OsAddSignalPendingFlag(struct TagTskCb *taskCb, signalInfo *info);
extern void OsSignalWaitReSche(struct TagTskCb *taskCb, U32 taskStatus);
extern void OsHandleOneSignal(struct TagTskCb *runTsk, int signum);
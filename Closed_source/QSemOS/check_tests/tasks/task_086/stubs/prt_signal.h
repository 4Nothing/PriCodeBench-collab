#pragma once
#include "auto_stub.h"

// Section and inline macros used in source
#define OS_SEC_L4_TEXT
#define OS_SEC_ALW_INLINE
#define INLINE inline __attribute__((always_inline))

// Signal-specific types and constants
#define PRT_SIGNAL_MAX 64
#define OS_SIGNAL_WAIT_FOREVER 0xFFFFFFFFu

// Task status flags used by signal code
#undef OS_TSK_TIMEOUT
#define OS_TSK_TIMEOUT 0x1
#undef OS_TSK_WAIT_SIGNAL
#define OS_TSK_WAIT_SIGNAL 0x2
#define OS_TSK_READY 0x4
#define OS_TSK_DELAY 0x8
#define OS_TSK_SIG_PAUSE 0x10
#define OS_TSK_DELAY_INTERRUPTIBLE 0x20
#define OS_TSK_HOLD_SIGNAL 0x40
#define OS_TSK_SUSPEND 0x80

#define TSK_STATUS_SET(t, f) ((t)->taskStatus |= (f))
#define TSK_STATUS_CLEAR(t, f) ((t)->taskStatus &= ~(f))

// Signal helpers (signalSet is defined in auto_stub.h as U32)
static inline int sigValid(int s) { return s >= 0 && s < (int)PRT_SIGNAL_MAX; }
static inline signalSet sigMask(int s) { return (signalSet)1u << (s & 31); }

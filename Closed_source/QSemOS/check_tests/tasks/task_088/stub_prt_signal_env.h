#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auto_stub.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Section and inline macros used in source
#define OS_SEC_L4_TEXT
#define OS_SEC_ALW_INLINE
#define INLINE

// Use struct TagTskCb from auto_stub.h
    typedef struct TagTskCb TagTskCb;

// Signal helpers - use static to avoid multiple definition
    static signalSet sigMask(int s) { return (signalSet)1ull << (s & 63); }
    static int sigValid(int s) { return s >= 0 && s < (int)PRT_SIGNAL_MAX; }

// Minimal list helpers - use ListInit which is not in auto_stub.h
    static inline void ListInit(struct TagListObject *head)
    {
        head->next = head;
        head->prev = head;
    }

// Stubs for memory allocation - use auto_stub.h versions
// Override OS_MID_SIGNAL and OS_MEM_DEFAULT_FSC_PT
#ifndef OS_MID_SIGNAL
#define OS_MID_SIGNAL 0x10
#endif
#ifndef OS_MEM_DEFAULT_FSC_PT
#define OS_MEM_DEFAULT_FSC_PT 0
#endif

    // Spinlocks and ints - additional functions not in auto_stub.h
    static inline uintptr_t OsIntLock(void) { return 0; }
    static inline void OsIntRestore(uintptr_t s) { (void)s; }

// Task and scheduling flags + helpers - use auto_stub.h versions where available

// Additional flags not in auto_stub.h
#define OS_TSK_SIG_PAUSE 0x10
#define OS_TSK_DELAY_INTERRUPTIBLE 0x20
#define OS_TSK_HOLD_SIGNAL 0x40

#define OS_SIGNAL_WAIT_FOREVER 0xFFFFFFFFu

    typedef unsigned int TskHandle;

    // Global running task for tests
    extern struct TagTskCb g_runningTask;
    void StubResetTask(void);
#define RUNNING_TASK (&g_runningTask)

// TCB helpers
#define GET_TCB_HANDLE(id) (&g_runningTask)
#define CHECK_TSK_PID_OVERFLOW(id) (0)
#define TSK_IS_UNUSED(t) (0)

    static inline void OsTskReadyAddBgd(struct TagTskCb *t) { (void)t; }
    static inline uintptr_t *OsTskContextInit(U32 taskId, U32 curStackSize, uintptr_t *top, uintptr_t entry)
    {
        (void)taskId;
        (void)curStackSize;
        (void)top;
        (void)entry;
        return NULL;
    }
    static inline void OsTskContextLoad(uintptr_t tsk) { (void)tsk; }
    static inline void OS_TSK_DELAY_LOCKED_DETACH(struct TagTskCb *t) { (void)t; }
    static inline void OsTskTimerAdd(struct TagTskCb *t, uintptr_t to) { (void)t; (void)to; }

// System error macro stubbed to no-op for tests
#ifndef OS_GOTO_SYS_ERROR1
#define OS_GOTO_SYS_ERROR1() \
    do                       \
    {                        \
    } while (0)
#endif

#ifdef __cplusplus
}
#endif
#pragma once
#include "auto_stub.h"
#include "stubs/prt_signal.h"  // For sigMask macro

#ifdef __cplusplus
extern "C"
{
#endif

    // Global running task for tests
    extern struct TagTskCb g_runningTask;
    void StubResetTask(void);
    void add_pending_signal(struct TagTskCb *task, int signo);
#define RUNNING_TASK (&g_runningTask)

// TCB helpers
#define GET_TCB_HANDLE(id) (&g_runningTask)
#define CHECK_TSK_PID_OVERFLOW(id) (0)
#define TSK_IS_UNUSED(t) (0)

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

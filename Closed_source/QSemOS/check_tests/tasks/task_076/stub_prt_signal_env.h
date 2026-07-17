#pragma once
#include "auto_stub.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Section and inline macros used in source
#define OS_SEC_L4_TEXT
#define OS_SEC_ALW_INLINE

// Signal helpers - defined in stubs/prt_signal.h

// Global running task for tests (defined in auto_stub.h)
void StubResetTask(void);
void RESET_COUNTERS(void);
void add_pending_signal(struct TagTskCb *task, int signum);
void print_result_int(const char *case_name, int expected, int actual);
void print_result_ptr(const char *case_name, void *expected, void *actual);
void print_pass_rate(void);

// TCB helpers
#define GET_TCB_HANDLE(id) (&g_runningTask)
#define CHECK_TSK_PID_OVERFLOW(id) (0)
#define TSK_IS_UNUSED(t) (0)

#ifdef __cplusplus
}
#endif
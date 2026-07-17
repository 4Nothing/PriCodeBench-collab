#pragma once
#include "auto_stub.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Section and inline macros used in source
#define OS_SEC_L4_TEXT
#define OS_SEC_ALW_INLINE

// List helpers (for signal info list) - auto_stub.h has ListDelete and ListTailAdd but not ListInit
#ifndef ListInit
static inline void ListInit(struct TagListObject *head)
{
    head->next = head;
    head->prev = head;
}
#endif

// Signal helpers - defined in auto_stub.h

// Global running task for tests (defined in auto_stub.h)
void StubResetTask(void);
void RESET_COUNTERS(void);
void add_pending_signal(struct TagTskCb *task, int signum);
void print_result_int(const char *name, int expected, int actual);
void print_pass_rate(void);

#ifdef __cplusplus
}
#endif
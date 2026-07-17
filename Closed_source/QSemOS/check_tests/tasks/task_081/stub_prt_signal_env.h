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

// Minimal list helpers compatible with usage pattern
static inline void ListInit(struct TagListObject *head)
{
    head->next = head;
    head->prev = head;
}

// Signal helpers - defined in auto_stub.h

// Global running task for tests
extern struct TagTskCb g_runningTask;
void StubResetTask(void);
void RESET_COUNTERS(void);
void add_pending_signal(struct TagTskCb *task, int signum);
void print_result_int(const char *name, int expected, int actual);
void print_result_ptr(const char *name, void *expected, void *actual);
void print_pass_rate(void);

#ifdef __cplusplus
}
#endif

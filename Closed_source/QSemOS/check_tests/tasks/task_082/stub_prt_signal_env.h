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

// Signal helpers
static inline int sigValid(int s) { return s >= 0 && s < (int)PRT_SIGNAL_MAX; }
static inline signalSet sigMask(int s) { return (signalSet)1ull << (s & 63); }

// Global running task for tests
extern struct TagTskCb g_runningTask;
void StubResetTask(void);

#ifdef __cplusplus
}
#endif

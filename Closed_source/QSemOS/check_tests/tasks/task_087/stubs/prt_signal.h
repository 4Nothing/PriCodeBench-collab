#pragma once
#include "auto_stub.h"
#include "../stub_prt_signal_env.h"

// Section and inline macros used in source
#define OS_SEC_L4_TEXT
#define OS_SEC_ALW_INLINE
// INLINE is defined in stub_prt_signal_env.h as empty to make functions non-inline

// Signal-specific types and constants (already defined in stub_prt_signal_env.h)
#ifndef OS_SIGNAL_WAIT_FOREVER
#define OS_SIGNAL_WAIT_FOREVER 0xFFFFFFFFu
#endif

// Task status flags used by signal code (already defined in stub_prt_signal_env.h)
// These are overridden here for compatibility

// Signal helpers (defined in stub_prt_signal_env.h)

// Test helper macros
#define RESET_COUNTERS() do { } while(0)

// Test helper functions
static inline void print_result_int(const char *case_name, U32 expected, U32 actual)
{
    printf("[CASE] %s\n", case_name);
    printf("  Expected: 0x%08X\n", expected);
    printf("  Actual  : 0x%08X\n", actual);
    printf("  Result  : %s\n\n", (expected == actual) ? "PASS" : "FAIL");
}

static inline void print_result_hexu32(const char *case_name, U32 expected, U32 actual)
{
    printf("[CASE] %s\n", case_name);
    printf("  Expected: 0x%08X\n", expected);
    printf("  Actual  : 0x%08X\n", actual);
    printf("  Result  : %s\n\n", (expected == actual) ? "PASS" : "FAIL");
}

static inline void print_pass_rate(void)
{
    printf("Test completed.\n");
}

// Additional signal functions needed by source
static inline void add_pending_signal(struct TagTskCb *task, int signum)
{
    task->sigPending |= sigMask(signum);
}

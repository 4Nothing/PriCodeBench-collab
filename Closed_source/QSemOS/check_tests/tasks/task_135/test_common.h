#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "auto_stub.h"
// REMOVED bad include

/* Extern declarations for stubs.c variables */
extern SwPmu g_perfTimed;
extern U32 g_swtPmuId;
extern U32 g_stub_timer_create_ret;
extern U32 g_stub_timer_start_ret;
extern U32 g_stub_timer_delete_ret;
extern U32 g_stub_next_timer_id;
extern U32 g_stub_created_timer_id;
extern struct TimerCreatePara g_last_timer_para;

/* Function declarations */
void OsPerfSwtimer(TimerHandle handle, U32 arg1, U32 arg2, U32 arg3, U32 arg4);


static int g_total = 0;
static int g_pass = 0;

static void print_result(const char *case_name, U32 expected, U32 actual)
{
    printf("[CASE] %s\n", case_name);
    printf("  Expected: 0x%08X\n", expected);
    printf("  Actual  : 0x%08X\n", actual);
    int ok = (expected == actual);
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL");
    g_total++;
    g_pass += ok;
}

static int print_summary()
{
    double rate = g_total ? (100.0 * g_pass / g_total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, g_pass, g_total);
    return (g_pass == g_total) ? 0 : 1;
}

/* print_case macro for task tests */
#define print_case(name, expected, actual) do { \
    g_total++; \
    int ok = ((expected) == (actual)); \
    if (ok) g_pass++; \
    printf("[CASE] %s\n", name); \
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL"); \
} while(0)

/* print_rate macro - calls print_summary */
#define print_rate() print_summary()

#endif // TEST_COMMON_H

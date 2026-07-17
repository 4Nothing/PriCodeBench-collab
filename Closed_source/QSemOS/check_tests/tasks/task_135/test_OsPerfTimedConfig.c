#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "auto_stub.h"
#include "prt_timer.h"
#include "prt_perf_pmu.h"

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

#define print_case(name, expected, actual) do { \
    g_total++; \
    int ok = ((expected) == (actual)); \
    if (ok) g_pass++; \
    printf("[%s] expect=%u actual=%u -> %s\n", name, expected, actual, ok ? "PASS" : "FAIL"); \
} while(0)

#define print_rate() print_summary()

#include "func_under_test.c"

int main(void)
{
    g_perfTimed.pmu.events.nr = 2;
    g_perfTimed.pmu.events.per[0].eventId = PERF_COUNT_CPU_CLOCK;
    g_perfTimed.pmu.events.per[0].period = 10;
    g_perfTimed.pmu.events.per[1].eventId = 0x2222;
    g_perfTimed.pmu.events.per[1].period = 0;

    g_stub_timer_create_ret = OS_OK;
    g_stub_next_timer_id = 1;
    U32 ret = OsPerfTimedConfig();
    print_case("create ok", OS_OK, ret);

    g_stub_timer_create_ret = 0xBEEFU;
    ret = OsPerfTimedConfig();
    print_case("create fails", 0xBEEFU, ret);

    g_perfTimed.pmu.events.per[0].period = 0;
    g_stub_timer_create_ret = OS_OK;
    ret = OsPerfTimedConfig();
    print_case("period invalid", OS_ERRNO_TIMER_INTERVAL_INVALID, ret);

    g_perfTimed.pmu.events.per[0].eventId = 0x1111;
    g_perfTimed.pmu.events.per[0].period = 10;
    ret = OsPerfTimedConfig();
    print_case("no cpu clock", OS_ERROR, ret);

    g_perfTimed.pmu.events.per[0].eventId = PERF_COUNT_CPU_CLOCK;
    g_perfTimed.pmu.events.per[0].period = 25;
    g_stub_timer_create_ret = OS_OK;
    (void)OsPerfTimedConfig();
    print_case("timer type ok", OS_TIMER_SOFTWARE, g_last_timer_para.type);
    print_case("timer mode ok", OS_TIMER_LOOP, g_last_timer_para.mode);
    print_case("timer interval ok", 25U, g_last_timer_para.interval);

    print_rate();
    return 0;
}

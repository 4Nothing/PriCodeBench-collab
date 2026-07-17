#include "test_common.h"
#include "func_under_test.c"

int main(void)
{
    /* Prepare events: nr=2 with some periods and ids */
    g_perfTimed.pmu.events.nr = 2;
    g_perfTimed.pmu.events.per[0].eventId = PERF_COUNT_CPU_CLOCK;
    g_perfTimed.pmu.events.per[0].period = 10;
    g_perfTimed.pmu.events.per[1].eventId = 0x1234;
    g_perfTimed.pmu.events.per[1].period = 0;

    /* Case 1: start ok (same core as primary) */
    set_core_id(0);
    g_primaryCoreId = 0;
    g_stub_timer_start_ret = OS_OK;
    U32 ret = OsPerfTimedStart();
    print_case("start ok", OS_OK, ret == OS_OK);

    /* Case 2: start timer fails -> expect error code and delete attempted */
    set_core_id(0);
    g_stub_timer_start_ret = 0xDEADU; /* make start fail */
    ret = OsPerfTimedStart();
    print_case("timer start fails", 0xDEADU, ret == 0xDEADU);

    /* Case 3: SMP non-primary core should early return OS_OK (if compiled w/o OS_OPTION_SMP it's also OK) */
    set_core_id(1);
    g_primaryCoreId = 0;
    g_stub_timer_start_ret = OS_OK;
    ret = OsPerfTimedStart();
    print_case("non-primary core", OS_OK, ret == OS_OK);

    /* Case 4: verify counts reset for current core */
    set_core_id(0);
    g_perfTimed.pmu.events.per[0].count[0] = 1234; /* seed */
    ret = OsPerfTimedStart();
    print_case("counts reset", OS_OK, ret == OS_OK);
    printf("  Expected count[0]: %u\n", 0U);
    printf("  Actual   count[0]: %llu\n\n", (unsigned long long)g_perfTimed.pmu.events.per[0].count[0]);

    /* Case 5: multiple events counts reset */
    g_perfTimed.pmu.events.per[1].count[0] = 77;
    ret = OsPerfTimedStart();
    print_case("counts reset multi", OS_OK, ret == OS_OK);
    printf("  Expected per[1].count[0]: %u\n", 0U);
    printf("  Actual   per[1].count[0]: %llu\n\n", (unsigned long long)g_perfTimed.pmu.events.per[1].count[0]);

    print_rate();
    return 0;
}

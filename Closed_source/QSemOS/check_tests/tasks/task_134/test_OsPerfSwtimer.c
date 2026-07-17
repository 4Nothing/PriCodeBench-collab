#include "test_common.h"

/* Stub for OsPerfTimedHandle - called by OsPerfSwtimer */
void OsPerfTimedHandle(void) {
    U32 i;
    for (i = 0; i < g_perfTimed.pmu.events.nr; i++) {
        OsPerfUpdateEventCount(&g_perfTimed.pmu.events.per[i], 1);
        OsPerfHandleOverFlow(&g_perfTimed.pmu.events.per[i], NULL);
    }
}

#include "func_under_test.c"

int main(void)
{
    g_perfTimed.pmu.events.nr = 1;
    g_perfTimed.pmu.events.per[0].eventId = 0x1234;
    memset(g_perfTimed.pmu.events.per[0].count, 0, sizeof(g_perfTimed.pmu.events.per[0].count));

    g_update_count_calls = g_overflow_calls = 0;

    OsPerfSwtimer(0, 0, 0, 0, 0);
    print_case("one trigger update>=1", 1, (g_update_count_calls >= 1));

    OsPerfSwtimer(0, 0, 0, 0, 0);
    OsPerfSwtimer(0, 0, 0, 0, 0);
    print_case("three triggers update>=3", 1, (g_update_count_calls >= 3));

    print_case("count[0]>=1", 1, (g_perfTimed.pmu.events.per[0].count[0] >= 1));

    print_case("overflow>=3", 1, (g_overflow_calls >= 3));

    OsPerfSwtimer((TimerHandle)0, 0, 0, 0, 0);
    print_case("no crash placeholder", 1, 1);

    print_rate();
    return 0;
}

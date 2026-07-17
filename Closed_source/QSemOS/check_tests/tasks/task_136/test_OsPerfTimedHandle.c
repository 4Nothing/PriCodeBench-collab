#include "test_common.h"
#include "func_under_test.c"

int main(void)
{
    /* Prepare one event */
    g_perfTimed.pmu.events.nr = 1;
    g_perfTimed.pmu.events.per[0].eventId = 0x1234;
    g_perfTimed.pmu.events.per[0].period = 0;
    memset(g_perfTimed.pmu.events.per[0].count, 0, sizeof(g_perfTimed.pmu.events.per[0].count));

    g_update_count_calls = g_overflow_calls = 0;

    /* Case 1: handle increments count on core 0 */
    set_core_id(0);
    OsPerfTimedHandle();
    print_case("inc core0", 1, (unsigned)g_perfTimed.pmu.events.per[0].count[0]);

    /* Case 2: handle increments count on core 1 */
    set_core_id(1);
    OsPerfTimedHandle();
    print_case("inc core1", 1, (unsigned)g_perfTimed.pmu.events.per[0].count[1]);

    /* Case 3: two more increments on core0 */
    set_core_id(0);
    OsPerfTimedHandle();
    OsPerfTimedHandle();
    print_case("inc twice core0", 3, (unsigned)g_perfTimed.pmu.events.per[0].count[0]);

    /* Case 4: overflow handler called count == number of events per call */
    print_case("overflow calls>=3", 1, (g_overflow_calls >= 4) ? 1U : 0U);

    /* Case 5: update_count_calls equals total OsPerfTimedHandle invocations times eventNum */
    print_case("update calls>=4", 1, (g_update_count_calls >= 4) ? 1U : 0U);

    print_rate();
    return 0;
}

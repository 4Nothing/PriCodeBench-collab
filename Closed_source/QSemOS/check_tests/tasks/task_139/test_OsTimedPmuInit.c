#include "test_common.h"

/* Stubs for cross-referenced functions */
U32 OsPerfTimedConfig(void) { return 0; }
U32 OsPerfTimedStart(void) { return 0; }
U32 OsPerfTimedStop(void) { return 0; }
char *OsPerfGetEventName(Event *event) { (void)event; return "timed"; }

/* g_perfTimed is defined in stubs.c */
extern SwPmu g_perfTimed;

#include "func_under_test.c"

int main(void)
{
    /* Case 1: init returns OS_OK */
    U32 ret = OsTimedPmuInit();
    print_case("init ok", OS_OK, ret);

    /* Case 2: pmu.type set */
    print_case("type set", PERF_EVENT_TYPE_TIMED, g_perfTimed.pmu.type);

    /* Case 3: function pointers set */
    print_case("config set", 1, (g_perfTimed.pmu.config != NULL) ? 1 : 0);
    print_case("start set", 1, (g_perfTimed.pmu.start != NULL) ? 1 : 0);

    /* Case 4: stop and getName set */
    print_case("stop set", 1, (g_perfTimed.pmu.stop != NULL) ? 1 : 0);
    print_case("getName set", 1, (g_perfTimed.pmu.getName != NULL) ? 1 : 0);

    /* Case 5: events cleared to zero */
    U32 sum = 0;
    for (int i = 0; i < 8; ++i)
        sum += (U32)g_perfTimed.pmu.events.per[i].eventId;
    print_case("events cleared", 0U, sum);

    print_rate();
    return 0;
}
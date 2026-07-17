#include "test_common.h"
#include "func_under_test.c"

/* External variables from test_stubs.c */
extern int tests_run;
extern int tests_passed;

/* External function from stubs.c */
extern char* OsPerfGetEventName(Event *event);

int main(void)
{
    Event e;

    /* Case 1: PERF_COUNT_CPU_CLOCK -> "timed" */
    e.eventId = PERF_COUNT_CPU_CLOCK;
    char *name = OsPerfGetEventName(&e);
    printf("Expected: timed\n");
    printf("Actual  : %s\n\n", name);
    tests_run++;
    if (name && strcmp(name, "timed") == 0)
        tests_passed++;

    /* Case 2: unknown id -> "unknown" */
    e.eventId = 0xDEAD;
    name = OsPerfGetEventName(&e);
    printf("Expected: unknown\n");
    printf("Actual  : %s\n\n", name);
    tests_run++;
    if (name && strcmp(name, "unknown") == 0)
        tests_passed++;

    /* Case 3: boundary value 0xFFFFFFFF (treated unknown) */
    e.eventId = 0xFFFFFFFFU;
    name = OsPerfGetEventName(&e);
    printf("Expected: unknown\n");
    printf("Actual  : %s\n\n", name);
    tests_run++;
    if (name && strcmp(name, "unknown") == 0)
        tests_passed++;

    /* Case 4: value 0x0 (which equals PERF_COUNT_CPU_CLOCK here) */
    e.eventId = 0x0U;
    name = OsPerfGetEventName(&e);
    printf("Expected: timed\n");
    printf("Actual  : %s\n\n", name);
    tests_run++;
    if (name && strcmp(name, "timed") == 0)
        tests_passed++;

    /* Case 5: alternating check back to unknown */
    e.eventId = 1U;
    name = OsPerfGetEventName(&e);
    printf("Expected: unknown\n");
    printf("Actual  : %s\n\n", name);
    tests_run++;
    if (name && strcmp(name, "unknown") == 0)
        tests_passed++;

    print_rate();
    return 0;
}

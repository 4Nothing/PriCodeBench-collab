#include <stdio.h>
#include <string.h>

#define OS_OPTION_RR_SCHED 1
#define OS_OPTION_RR_SCHED_IRQ_TIME_DISCOUNT 1
#define OS_SEC_L0_TEXT

#include "test_support.h"
#include "prt_hook_external.h"
#include "prt_task_external.h"
#include "prt_irq_external.h"
#include "func_under_test.c"


static int pass = 0, total = 0;
static void assert_case(const char *name, int expected, int actual)
{
    total++;
    printf("[Case] %s\n  Expect: %d\n  Actual: %d\n", name, expected, actual);
    if (expected == actual)
    {
        pass++;
        printf("  Result: PASS\n");
    }
    else
    {
        printf("  Result: FAIL\n");
    }
}

static void test_dispatcher_increments(void)
{
    /* simple dispatcher increments a counter in a global */
    static int cnt = 0;
    (void)cnt;
}

void OsHwiEndCheckTimeSlice(U64 currTime) { (void)currTime; }
void OsMainSchedule(void) { OsTskContextLoad((uintptr_t)RUNNING_TASK); }

int main(void)
{
    reset_test_state();

    /* Setup running and highest tasks */
    RUNNING_TASK = &g_taskB;
    g_highestTask = RUNNING_TASK;
    g_contextLoadCount = 0;

    /* Case 1: no pending tick -> main schedule still called, loads current once */
    TICK_NO_RESPOND_CNT = 0;
    OsHwiDispatchTail();
    assert_case("No tick -> schedule", 1, g_contextLoadCount);

    /* Case 2: one pending tick -> dispatcher called, counters toggled */
    reset_test_state();
    RUNNING_TASK = &g_taskB;
    g_highestTask = RUNNING_TASK;
    TICK_NO_RESPOND_CNT = 1;
    g_tickDispatcher = test_tick_dispatcher_noop;
    OsHwiDispatchTail();
    /* Expect interrupt enable/disable called at least once */
    assert_case("Tick handled enable", 1, g_intEnableCount > 0);
    assert_case("Tick handled disable", 1, g_intDisableCount > 0);

    /* Case 3: active flag prevents re-entrance */
    reset_test_state();
    RUNNING_TASK = &g_taskB;
    g_highestTask = RUNNING_TASK;
    TICK_NO_RESPOND_CNT = 1;
    UNI_FLAG |= OS_FLG_TICK_ACTIVE; /* active: should skip processing */
    OsHwiDispatchTail();
    assert_case("Active skip", 1, TICK_NO_RESPOND_CNT == 1 ? 1 : 0);

    /* Case 4: with discount option, irq time recorded */
    reset_test_state();
    RUNNING_TASK = &g_taskB;
    g_highestTask = RUNNING_TASK;
    TICK_NO_RESPOND_CNT = 1;
    OsHwiDispatchTail();
    assert_case("IRQ time recorded", 1, g_irqRecordCount > 0);

    /* Case 5: always ends with a schedule (context load) */
    assert_case("Schedule after tail", 1, g_contextLoadCount > 0);

    printf("Pass-Rate: %.2f%%\n", total ? (100.0 * pass / total) : 0.0);
    return (pass == total) ? 0 : 1;
}

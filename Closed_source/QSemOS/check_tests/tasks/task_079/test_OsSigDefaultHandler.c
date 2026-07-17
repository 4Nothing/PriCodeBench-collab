#include "test_common.h"
#include "stubs/prt_signal.h"
#include "func_under_test.c"

int main(void)
{
    RESET_COUNTERS();
    StubResetTask();

    // Case 1: call with 0, expect no state change
    U32 before = RUNNING_TASK->sigPending;
    OsSigDefaultHandler(0);
    print_result_hexu32("default-0 no-change", before, RUNNING_TASK->sigPending);

    // Case 2: call with arbitrary positive
    before = RUNNING_TASK->sigPending;
    OsSigDefaultHandler(7);
    print_result_hexu32("default-7 no-change", before, RUNNING_TASK->sigPending);

    // Case 3: call with near-max
    before = RUNNING_TASK->sigPending;
    OsSigDefaultHandler(PRT_SIGNAL_MAX - 1);
    print_result_hexu32("default-max-1 no-change", before, RUNNING_TASK->sigPending);

    // Case 4: call with large number (still no-op)
    before = RUNNING_TASK->sigPending;
    OsSigDefaultHandler(1234);
    print_result_hexu32("default-1234 no-change", before, RUNNING_TASK->sigPending);

    // Case 5: call while some pending exists
    RUNNING_TASK->sigPending = 0xAA55AA55u;
    OsSigDefaultHandler(1);
    print_result_hexu32("default-keep-pending", 0xAA55AA55u, RUNNING_TASK->sigPending);

    print_pass_rate();
    return (g_pass == g_total) ? 0 : 1;
}

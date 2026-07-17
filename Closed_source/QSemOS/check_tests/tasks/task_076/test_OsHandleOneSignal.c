#include "test_common.h"
#include "stubs/prt_signal.h"
#include "func_under_test.c"

static int g_called = 0;
static void handler_inc(int s)
{
    (void)s;
    ++g_called;
}

int main(void)
{
    RESET_COUNTERS();
    StubResetTask();
    g_called = 0;

    // Prepare pending for 5 with handler
    add_pending_signal(RUNNING_TASK, 5);
    RUNNING_TASK->sigVectors[5] = handler_inc;

    // Case 1: handle present signal -> handler called and cleared
    OsHandleOneSignal(RUNNING_TASK, 5);
    print_result_int("handler called once", 1, g_called);
    print_result_int("pending cleared 5", 0, (RUNNING_TASK->sigPending & sigMask(5)) != 0);

    // Case 2: handle non-pending -> no change
    OsHandleOneSignal(RUNNING_TASK, 6);
    print_result_int("still called once", 1, g_called);

    // Case 3: handler NULL -> still clears pending after call
    add_pending_signal(RUNNING_TASK, 2);
    RUNNING_TASK->sigVectors[2] = NULL;
    OsHandleOneSignal(RUNNING_TASK, 2);
    print_result_int("pending cleared 2", 0, (RUNNING_TASK->sigPending & sigMask(2)) != 0);

    // Case 4: multiple pendings, only matching clears
    add_pending_signal(RUNNING_TASK, 1);
    add_pending_signal(RUNNING_TASK, 3);
    RUNNING_TASK->sigVectors[1] = handler_inc;
    RUNNING_TASK->sigVectors[3] = handler_inc;
    OsHandleOneSignal(RUNNING_TASK, 1);
    print_result_int("1 cleared", 0, (RUNNING_TASK->sigPending & sigMask(1)) != 0);
    print_result_int("3 stays", 1, (RUNNING_TASK->sigPending & sigMask(3)) != 0);

    // Case 5: handle remaining 3
    OsHandleOneSignal(RUNNING_TASK, 3);
    print_result_int("3 cleared", 0, (RUNNING_TASK->sigPending & sigMask(3)) != 0);

    print_pass_rate();
    return (g_pass == g_total) ? 0 : 1;
}

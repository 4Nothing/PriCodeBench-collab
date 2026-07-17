#include "test_common.h"
#include "stubs/prt_signal.h"
#include "func_under_test.c"

static int g_count = 0;
static void hcount(int s)
{
    (void)s;
    ++g_count;
}

int main(void)
{
    RESET_COUNTERS();
    StubResetTask();
    g_count = 0;

    // Pending signals 1,2,4
    add_pending_signal(RUNNING_TASK, 1);
    add_pending_signal(RUNNING_TASK, 2);
    add_pending_signal(RUNNING_TASK, 4);
    RUNNING_TASK->sigVectors[1] = hcount;
    RUNNING_TASK->sigVectors[2] = hcount;
    RUNNING_TASK->sigVectors[4] = hcount;

    // Block 1 and 4, unblock 2
    RUNNING_TASK->sigMask = sigMask(1) | sigMask(4);
    OsHandleUnBlockSignal(RUNNING_TASK);
    print_result_int("handler executed once (2)", 1, g_count);
    print_result_int("2 cleared", 0, (RUNNING_TASK->sigPending & sigMask(2)) != 0);
    print_result_int("1 still pending", 1, (RUNNING_TASK->sigPending & sigMask(1)) != 0);

    // Unblock all
    RUNNING_TASK->sigMask = 0;
    OsHandleUnBlockSignal(RUNNING_TASK);
    print_result_int("two more executed", 3, g_count);
    print_result_int("1 cleared", 0, (RUNNING_TASK->sigPending & sigMask(1)) != 0);
    print_result_int("4 cleared", 0, (RUNNING_TASK->sigPending & sigMask(4)) != 0);

    print_pass_rate();
    return (g_pass == g_total) ? 0 : 1;
}

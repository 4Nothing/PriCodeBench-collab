#include "test_common.h"
#include "stubs/prt_signal.h"
#include "func_under_test.c"

static signalInfo mkinfo(int s)
{
    signalInfo i;
    i.si_signo = s;
    return i;
}

int main(void)
{
    RESET_COUNTERS();
    StubResetTask();

    // Case 1: add new pending 4
    signalInfo i4 = mkinfo(4);
    U32 ret = OsAddSignalPendingFlag(RUNNING_TASK, &i4);
    print_result_int("add 4 -> OK", OS_OK, ret);
    print_result_int("pending has 4", 1, (RUNNING_TASK->sigPending & sigMask(4)) != 0);

    // Case 2: update existing 4 (should remain set)
    signalInfo i4b = mkinfo(4);
    ret = OsAddSignalPendingFlag(RUNNING_TASK, &i4b);
    print_result_int("update 4 -> OK", OS_OK, ret);
    print_result_int("pending still has 4", 1, (RUNNING_TASK->sigPending & sigMask(4)) != 0);

    // Case 3: add 0
    signalInfo i0 = mkinfo(0);
    ret = OsAddSignalPendingFlag(RUNNING_TASK, &i0);
    print_result_int("add 0 -> OK", OS_OK, ret);
    print_result_int("pending has 0", 1, (RUNNING_TASK->sigPending & sigMask(0)) != 0);

    // Case 4: add 31 (bit overlap boundary)
    signalInfo i31 = mkinfo(31);
    ret = OsAddSignalPendingFlag(RUNNING_TASK, &i31);
    print_result_int("add 31 -> OK", OS_OK, ret);
    print_result_int("pending has 31", 1, (RUNNING_TASK->sigPending & sigMask(31)) != 0);

    // Case 5: add 33 (wrap masked to bit1 in our mask helper) -> still treated as add
    signalInfo i33 = mkinfo(33);
    ret = OsAddSignalPendingFlag(RUNNING_TASK, &i33);
    print_result_int("add 33 -> OK", OS_OK, ret);

    print_pass_rate();
    return (g_pass == g_total) ? 0 : 1;
}

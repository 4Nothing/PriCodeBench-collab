#include "test_common.h"
#include "stubs/prt_signal.h"
#include "func_under_test.c"

int main(void)
{
    RESET_COUNTERS();
    StubResetTask();

    // Case 1: set forever -> clear timeout flag
    RUNNING_TASK->taskStatus |= OS_TSK_TIMEOUT;
    OsSignalTimeOutSet(RUNNING_TASK, OS_SIGNAL_WAIT_FOREVER);
    print_result_int("forever clears timeout", 0, (RUNNING_TASK->taskStatus & OS_TSK_TIMEOUT) != 0);

    // Case 2: set non-zero -> set timeout flag
    OsSignalTimeOutSet(RUNNING_TASK, 10);
    print_result_int("nonzero sets timeout", 1, (RUNNING_TASK->taskStatus & OS_TSK_TIMEOUT) != 0);

    // Case 3: set non-zero again keeps flag set
    OsSignalTimeOutSet(RUNNING_TASK, 5);
    print_result_int("nonzero keeps timeout", 1, (RUNNING_TASK->taskStatus & OS_TSK_TIMEOUT) != 0);

    // Case 4: then set forever clears
    OsSignalTimeOutSet(RUNNING_TASK, OS_SIGNAL_WAIT_FOREVER);
    print_result_int("forever clears after set", 0, (RUNNING_TASK->taskStatus & OS_TSK_TIMEOUT) != 0);

    // Case 5: zero treated as non-forever -> sets flag
    OsSignalTimeOutSet(RUNNING_TASK, 0);
    print_result_int("zero sets timeout", 1, (RUNNING_TASK->taskStatus & OS_TSK_TIMEOUT) != 0);

    print_pass_rate();
    return (g_pass == g_total) ? 0 : 1;
}

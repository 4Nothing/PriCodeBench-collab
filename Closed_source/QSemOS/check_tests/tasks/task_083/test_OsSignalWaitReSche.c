#include "test_common.h"
#include "stubs/prt_signal.h"
#include "func_under_test.c"

int main(void)
{
    RESET_COUNTERS();
    StubResetTask();

    // Case 1: with timeout flag -> clears it
    RUNNING_TASK->taskStatus |= OS_TSK_TIMEOUT;
    OsSignalWaitReSche(RUNNING_TASK, RUNNING_TASK->taskStatus);
    print_result_int("clears timeout", 0, (RUNNING_TASK->taskStatus & OS_TSK_TIMEOUT) != 0);

    // Case 2: not suspended -> would add to ready bgd (no-op), status unchanged
    U32 before = RUNNING_TASK->taskStatus;
    OsSignalWaitReSche(RUNNING_TASK, before);
    print_result_hexu32("status unchanged", before, RUNNING_TASK->taskStatus);

    // Case 3: suspended -> no bgd add (still no-op), state unchanged
    RUNNING_TASK->taskStatus |= OS_TSK_SUSPEND;
    before = RUNNING_TASK->taskStatus;
    OsSignalWaitReSche(RUNNING_TASK, RUNNING_TASK->taskStatus);
    print_result_hexu32("suspend unchanged", before, RUNNING_TASK->taskStatus);

    // Case 4: combine flags, ensure no crash
    RUNNING_TASK->taskStatus |= (OS_TSK_READY | OS_TSK_DELAY);
    before = RUNNING_TASK->taskStatus;
    OsSignalWaitReSche(RUNNING_TASK, before);
    print_result_hexu32("combined unchanged", before, RUNNING_TASK->taskStatus);

    // Case 5: clear suspend and test again
    RUNNING_TASK->taskStatus &= ~OS_TSK_SUSPEND;
    before = RUNNING_TASK->taskStatus;
    OsSignalWaitReSche(RUNNING_TASK, before);
    print_result_hexu32("after clear suspend", before, RUNNING_TASK->taskStatus);

    print_pass_rate();
    return (g_pass == g_total) ? 0 : 1;
}

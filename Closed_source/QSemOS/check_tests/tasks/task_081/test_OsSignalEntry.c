#include "test_common.h"
#include "stubs/prt_signal.h"
#include "func_under_test.c"

static int g_flag = 0;
static void hset(int s)
{
    (void)s;
    g_flag = 42;
}

int main(void)
{
    RESET_COUNTERS();
    StubResetTask();
    g_flag = 0;

    // Prepare pending and context
    add_pending_signal(RUNNING_TASK, 9);
    RUNNING_TASK->sigVectors[9] = hset;
    RUNNING_TASK->holdSignal = 9;
    RUNNING_TASK->taskStatus |= OS_TSK_HOLD_SIGNAL;
    RUNNING_TASK->oldStackPointer = (uintptr_t *)0x12345678;
    RUNNING_TASK->stackPointer = (uintptr_t *)0xABCDEF00;

    OsSignalEntry(RUNNING_TASK->taskPid);

    // Case 1: handler executed
    print_result_int("handler executed", 42, g_flag);
    // Case 2: status cleared
    print_result_int("hold flag cleared", 0, (RUNNING_TASK->taskStatus & OS_TSK_HOLD_SIGNAL) != 0);
    // Case 3: SP restored
    print_result_ptr("sp restored", (void *)0x12345678, RUNNING_TASK->stackPointer);
    // Case 4: holdSignal cleared
    print_result_int("holdSignal cleared", 0, RUNNING_TASK->holdSignal);
    // Case 5: oldStackPointer cleared
    print_result_ptr("oldSP cleared", NULL, RUNNING_TASK->oldStackPointer);

    print_pass_rate();
    return (g_pass == g_total) ? 0 : 1;
}

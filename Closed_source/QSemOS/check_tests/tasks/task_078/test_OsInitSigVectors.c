#include "test_common.h"
#include "stubs/prt_signal.h"
#include "../task_079/func_under_test.c"
#include "func_under_test.c"

static void dummy1(int s) { (void)s; }
static void dummy2(int s) { (void)s; }

int main(void)
{
    RESET_COUNTERS();
    StubResetTask();

    // Prepare mixed vectors
    RUNNING_TASK->sigVectors[0] = dummy1;
    RUNNING_TASK->sigVectors[1] = dummy2;
    RUNNING_TASK->sigVectors[2] = NULL;
    RUNNING_TASK->sigVectors[3] = SIG_DFL;
    RUNNING_TASK->sigVectors[4] = SIG_IGN;

    OsInitSigVectors(RUNNING_TASK);

    // After init, all should be OsSigDefaultHandler
    print_result_ptr("vec[0] default", OsSigDefaultHandler, RUNNING_TASK->sigVectors[0]);
    print_result_ptr("vec[1] default", OsSigDefaultHandler, RUNNING_TASK->sigVectors[1]);
    print_result_ptr("vec[2] default", OsSigDefaultHandler, RUNNING_TASK->sigVectors[2]);
    print_result_ptr("vec[10] default", OsSigDefaultHandler, RUNNING_TASK->sigVectors[10]);
    print_result_ptr("vec[last] default", OsSigDefaultHandler, RUNNING_TASK->sigVectors[PRT_SIGNAL_MAX - 1]);

    print_pass_rate();
    return (g_pass == g_total) ? 0 : 1;
}

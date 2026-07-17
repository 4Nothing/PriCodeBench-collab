#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

#include "auto_stub.h"
#include "func_under_test.c"


static int total = 0, passed = 0;

static int entry_called = 0;
static uintptr_t last_args[4];
static void dummy_entry(uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d)
{
    entry_called++;
    last_args[0] = a;
    last_args[1] = b;
    last_args[2] = c;
    last_args[3] = d;
}

static void setup_tcb_with_args(uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d)
{
    static struct TagTskCb cb;
    memset(&cb, 0, sizeof(cb));
    cb.taskEntry = dummy_entry;
    cb.args[0] = a;
    cb.args[1] = b;
    cb.args[2] = c;
    cb.args[3] = d;
    stub_current_tcb = &cb;
    entry_called = 0;
    stub_int_lock_count = 0;
    stub_int_restore_count = 0;
    stub_task_exit_arg = NULL;
    OS_TASK_LOCK_DATA = 1;
}

static void case_entry_called_with_args()
{
    setup_tcb_with_args(1, 2, 3, 4);
    OsTskEntry(0);
    total++;
    passed += run_check("entry called once", 1, entry_called);
    total++;
    passed += run_check("arg0", 1, last_args[0]);
    total++;
    passed += run_check("arg3", 4, last_args[3]);
}

static void case_lock_unlock_sequence()
{
    setup_tcb_with_args(0, 0, 0, 0);
    OsTskEntry(0);
    total++;
    passed += run_check("int lock called once", 1, stub_int_lock_count);
    total++;
    passed += run_check("int restore called once", 1, stub_int_restore_count);
}

static void case_task_lock_data_cleared()
{
    setup_tcb_with_args(0, 0, 0, 0);
    OsTskEntry(0);
    total++;
    passed += run_check("OS_TASK_LOCK_DATA cleared", 0, OS_TASK_LOCK_DATA);
}

static void case_task_exit_called()
{
    setup_tcb_with_args(0xA, 0xB, 0xC, 0xD);
    OsTskEntry(0);
    total++;
    passed += run_check("task exit got same tcb", (long long)(uintptr_t)stub_current_tcb, (long long)(uintptr_t)stub_task_exit_arg);
}

static void case_multiple_invocations()
{
    setup_tcb_with_args(10, 20, 30, 40);
    OsTskEntry(0);
    OsTskEntry(0);
    total++;
    passed += run_check("entry called twice", 2, entry_called);
}

int main()
{
    case_entry_called_with_args();
    case_lock_unlock_sequence();
    case_task_lock_data_cleared();
    case_task_exit_called();
    case_multiple_invocations();
    double pr = (total ? 100.0 * passed / total : 0.0);
    printf("Pass-Rate: %.2f%%\n", pr);
    return (passed == total) ? 0 : 1;
}

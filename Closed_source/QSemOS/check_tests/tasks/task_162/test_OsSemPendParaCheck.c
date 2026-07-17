#include <stdio.h>
#include "test_helpers.h"
#include "os_stub.h"
#include "func_under_test.c"

static void run_case(const char *name, U32 expected, U32 actual, int *pass, int *total)
{
    (*total)++;
    printf("%s => expected=%u, actual=%u\n", name, expected, actual);
    if (expected == actual)
        (*pass)++;
}

int main(void)
{
    test_reset();
    int pass = 0, total = 0;

    // 1) timeout==0 => UNAVAILABLE
    run_case("timeout zero", OS_ERRNO_SEM_UNAVAILABLE, OsSemPendParaCheck(0), &pass, &total);

    // 2) lock data non-zero => PEND_IN_LOCK
    g_task_lock_data = 1;
    run_case("task lock", OS_ERRNO_SEM_PEND_IN_LOCK, OsSemPendParaCheck(10), &pass, &total);

    // 3) lock zero and non-zero timeout => OK
    g_task_lock_data = 0;
    run_case("normal non-zero", OS_OK, OsSemPendParaCheck(5), &pass, &total);

    // 4) lock zero and forever => OK
    run_case("forever", OS_OK, OsSemPendParaCheck(OS_WAIT_FOREVER), &pass, &total);

    // 5) large timeout => OK
    run_case("large timeout", OS_OK, OsSemPendParaCheck(100000), &pass, &total);

    printf("Pass-Rate: %.2f%%\n", total ? (pass * 100.0 / total) : 0.0);
    return pass == total ? 0 : 1;
}

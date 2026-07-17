#define _POSIX_C_SOURCE 200112L
#include "test_support.h"
#include "prt_trace.h"
#include "prt_irq_internal.h"

extern U32 g_defHandlerHwiNum;
extern long long g_last_error_reported;

#define OS_REPORT_ERROR(err) (g_last_error_reported = (err))

#include "func_under_test.c"

int main(void)
{
    int passed = 0;
    int total = 0;
    U32 cases[5] = {0, 1, 7, OS_HWI_MAX_NUM - 1, 1234u % OS_HWI_MAX_NUM};

    for (int i = 0; i < 5; ++i)
    {
        g_last_error_reported = 0;
        g_defHandlerHwiNum = 0xFFFFFFFFu;
        OsHwiDefaultHandler((HwiArg)cases[i]);
        long long expectedErr = OS_ERRNO_HWI_UNCREATED;
        long long actualErr = g_last_error_reported;
        int pass1 = (actualErr == expectedErr);
        print_case_result("OsHwiDefaultHandler error", expectedErr, actualErr, pass1);
        passed += pass1;
        total++;

        long long expectedNum = OS_IRQ2HWI(cases[i]);
        long long actualNum = g_defHandlerHwiNum;
        int pass2 = (actualNum == expectedNum);
        char name[64];
        snprintf(name, sizeof(name), "def hwi num for %u", cases[i]);
        print_case_result(name, expectedNum, actualNum, pass2);
        passed += pass2;
        total++;
    }

    double rate = (total ? (100.0 * passed / total) : 0.0);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (passed == total) ? 0 : 1;
}

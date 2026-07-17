#include "test_support.h"
#include "prt_trace.h"
#include "prt_irq_internal.h"
#include "func_under_test.c"

int main(void)
{
    int passed = 0, total = 0;
    U32 irqs[5] = {0, 2, 5, OS_HWI_MAX_NUM - 2, OS_HWI_MAX_NUM - 1};

    for (int i = 0; i < 5; ++i)
    {
        U32 irq = irqs[i];
        OsHwiDescFieldInit(irq);
        long long expectedPtr = (long long)(uintptr_t)OsHwiDefaultHandler;
        long long actualPtr = (long long)(uintptr_t)OsHwiFuncGet(irq);
        int pass1 = (actualPtr == expectedPtr);
        char name1[80];
        snprintf(name1, sizeof(name1), "handler default at %u", irq);
        print_case_result(name1, expectedPtr, actualPtr, pass1);
        passed += pass1;
        total++;

        long long expectedArg = irq;
        long long actualArg = OsHwiParaGet(irq);
        int pass2 = (expectedArg == actualArg);
        char name2[80];
        snprintf(name2, sizeof(name2), "param = irq at %u", irq);
        print_case_result(name2, expectedArg, actualArg, pass2);
        passed += pass2;
        total++;
    }

    double rate = (total ? (100.0 * passed / total) : 0.0);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (passed == total) ? 0 : 1;
}

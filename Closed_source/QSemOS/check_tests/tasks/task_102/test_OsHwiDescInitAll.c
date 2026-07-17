#include "test_support.h"
#include "prt_trace.h"
#include "prt_irq_internal.h"
#include "func_under_test.c"

int main(void)
{
    int passed = 0, total = 0;

    /* dirty some entries */
    for (U32 i = 0; i < OS_HWI_MAX_NUM; ++i)
    {
        OsHwiFuncSet(i, NULL);
        OsHwiParaSet(i, 0xDEAD0000u + i);
    }

    OsHwiDescInitAll();

    U32 checkIdx[5] = {0, 1, OS_HWI_MAX_NUM / 2, OS_HWI_MAX_NUM - 2, OS_HWI_MAX_NUM - 1};
    for (int k = 0; k < 5; ++k)
    {
        U32 idx = checkIdx[k];
        long long expectedPtr = (long long)(uintptr_t)OsHwiDefaultHandler;
        long long actualPtr = (long long)(uintptr_t)OsHwiFuncGet(idx);
        int pass1 = (expectedPtr == actualPtr);
        char n1[64];
        snprintf(n1, sizeof(n1), "default handler %u", idx);
        print_case_result(n1, expectedPtr, actualPtr, pass1);
        passed += pass1;
        total++;

        long long expectedArg = idx;
        long long actualArg = OsHwiParaGet(idx);
        int pass2 = (expectedArg == actualArg);
        char n2[64];
        snprintf(n2, sizeof(n2), "param equals %u", idx);
        print_case_result(n2, expectedArg, actualArg, pass2);
        passed += pass2;
        total++;
    }

    double rate = (total ? (100.0 * passed / total) : 0.0);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (passed == total) ? 0 : 1;
}

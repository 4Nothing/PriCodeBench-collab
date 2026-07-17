#include "test_support.h"
#include "prt_trace.h"
#include "prt_irq_internal.h"
#include "func_under_test.c"

static U32 g_call_count = 0;
static U32 g_last_param = 0;
static void sample_isr(HwiArg arg)
{
    g_call_count++;
    g_last_param = arg;
}

int main(void)
{
    int passed = 0, total = 0;
    OsHwiDescInitAll();

    U32 irqs[5] = {0, 3, 4, OS_HWI_MAX_NUM - 2, OS_HWI_MAX_NUM - 1};
    for (int i = 0; i < 5; ++i)
    {
        U32 irq = irqs[i];
        OsHwiFuncSet(irq, sample_isr);
        OsHwiParaSet(irq, 0xABC00000u + irq);
        g_call_count = 0;
        g_last_param = 0;
        OsHwiHookDispatcher((HwiHandle)(irq + 16));
        long long expectedCount = 1;
        long long actualCount = g_call_count;
        char n1[64];
        snprintf(n1, sizeof(n1), "dispatch count for %u", irq);
        int pass1 = (actualCount == expectedCount);
        print_case_result(n1, expectedCount, actualCount, pass1);
        passed += pass1;
        total++;

        long long expectedParam = 0xABC00000u + irq;
        long long actualParam = g_last_param;
        char n2[64];
        snprintf(n2, sizeof(n2), "dispatch param for %u", irq);
        int pass2 = (actualParam == expectedParam);
        print_case_result(n2, expectedParam, actualParam, pass2);
        passed += pass2;
        total++;
    }

    double rate = (total ? (100.0 * passed / total) : 0.0);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (passed == total) ? 0 : 1;
}

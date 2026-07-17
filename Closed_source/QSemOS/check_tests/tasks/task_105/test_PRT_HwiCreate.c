#include "test_support.h"
#include "prt_trace.h"
#include "prt_irq_internal.h"

U32 OsHwiConnectHandle(U32 irqNum, HwiProcFunc handler, HwiArg arg);

/* Helper functions needed by PRT_HwiCreate */
void OsHwiDescFieldInit(U32 irqNum)
{
    OsHwiFuncSet(irqNum, OsHwiDefaultHandler);
    OsHwiParaSet(irqNum, irqNum);
}

void OsHwiDescInitAll(void)
{
    U32 irqNum;
    for (irqNum = 0; irqNum < OS_HWI_MAX_NUM; irqNum++) {
        OsHwiDescFieldInit(irqNum);
    }
}

void OsHwiHookDispatcher(HwiHandle archHwi)
{
    HwiHandle hwiNum = OS_HWI_GET_HWINUM(archHwi);
    U32 irqNum = OS_HWI2IRQ(hwiNum);

    OS_MHOOK_ACTIVATE_PARA1(OS_HOOK_HWI_ENTRY, hwiNum);

    OsHwiHandleActive(irqNum);

    OS_MHOOK_ACTIVATE_PARA1(OS_HOOK_HWI_EXIT, hwiNum);
}

#include "func_under_test.c"

static U32 g_called = 0;
static U32 g_arg_seen = 0;
static void isr(HwiArg a)
{
    g_called++;
    g_arg_seen = a;
}

int main(void)
{
    int passed = 0, total = 0;
    OsHwiDescInitAll();

    /* 1. Invalid hwi num */
    U32 ret = PRT_HwiCreate(OS_HWI_MAX_NUM, isr, 1);
    print_case_result("invalid hwi num", OS_ERRNO_HWI_NUM_INVALID, ret, ret == OS_ERRNO_HWI_NUM_INVALID);
    passed += (ret == OS_ERRNO_HWI_NUM_INVALID);
    total++;

    /* 2. NULL handler */
    ret = PRT_HwiCreate(0, NULL, 0);
    print_case_result("null handler", OS_ERRNO_HWI_PROC_FUNC_NULL, ret, ret == OS_ERRNO_HWI_PROC_FUNC_NULL);
    passed += (ret == OS_ERRNO_HWI_PROC_FUNC_NULL);
    total++;

    /* 3. Create ok */
    ret = PRT_HwiCreate(18, isr, 0x1234);
    print_case_result("create ok", OS_OK, ret, ret == OS_OK);
    passed += (ret == OS_OK);
    total++;
    long long expectedPtr = (long long)(uintptr_t)isr;
    long long actualPtr = (long long)(uintptr_t)OsHwiFuncGet(2);
    print_case_result("handler installed", expectedPtr, actualPtr, expectedPtr == actualPtr);
    passed += (expectedPtr == actualPtr);
    total++;

    /* 4. Duplicate create should fail */
    ret = PRT_HwiCreate(18, isr, 0x1234);
    print_case_result("duplicate create", OS_ERRNO_HWI_ALREADY_CREATED, ret, ret == OS_ERRNO_HWI_ALREADY_CREATED);
    passed += (ret == OS_ERRNO_HWI_ALREADY_CREATED);
    total++;

    /* 5. Dispatch calls installed handler */
    g_called = 0;
    g_arg_seen = 0;
    OsHwiHookDispatcher((HwiHandle)18);
    print_case_result("dispatch count", 1, g_called, g_called == 1);
    passed += (g_called == 1);
    total++;
    print_case_result("dispatch arg", 0x1234, g_arg_seen, g_arg_seen == 0x1234);
    passed += (g_arg_seen == 0x1234);
    total++;

    double rate = (total ? (100.0 * passed / total) : 0.0);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (passed == total) ? 0 : 1;
}

#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint32_t U32;
typedef uint16_t U16;
typedef uintptr_t HwiArg;
typedef U32 HwiHandle;
typedef U32 HwiPrior;
typedef U32 HwiMode;
typedef void (*HwiProcFunc)(HwiArg);

#define OS_OK 0
#define OS_HWI_MAX_NUM 128
#define OS_SEC_TEXT
#define OS_IRQ2HWI(irq) ((irq) + 16)
#define OS_HWI_GET_HWINUM(hwi) ((hwi) & 0xFF)
#define OS_HWI2IRQ(hwi) ((hwi) - 16)

#define TRUE 1
#define FALSE 0

#define OS_MHOOK_ACTIVATE_PARA1(hook, para) do { (void)hook; (void)para; } while(0)

struct TagHwiHandleForm {
    HwiProcFunc hwiHandler;
    HwiArg hwiPram;
};

struct TagHwiModeForm {
    U16 prior;
    U16 mode;
};

extern U32 g_defHandlerHwiNum;
extern void OsHwiDefaultHandler(HwiArg);
extern void OsHwiFuncSet(U32 irqNum, HwiProcFunc handler);
extern void OsHwiParaSet(U32 irqNum, HwiArg arg);
extern void OsHwiHandleActive(U32 irqNum);
extern void OsHwiDescInitAll(void);

static int assert_eq_u32(const char *name, U32 expect, U32 actual)
{
    printf("[CASE] %s\n  Expect: %u\n  Actual: %u\n", name, expect, actual);
    return expect == actual;
}

static void print_pass_rate(int pass, int total)
{
    double rate = total ? (100.0 * pass / total) : 0.0;
    printf("Pass-Rate: %.2f%%\n", rate);
}

static inline void print_case_result(const char *name, long long expected, long long actual, int passed)
{
    printf("[CASE] %s\n", name);
    printf("  Expected: %lld\n", expected);
    printf("  Actual  : %lld\n", actual);
    printf("  Result  : %s\n\n", passed ? "PASS" : "FAIL");
}

#endif /* TEST_SUPPORT_H */

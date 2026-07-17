#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint32_t U32;
typedef uint16_t U16;
typedef uintptr_t HwiArg;
typedef U32 HwiHandle;
typedef U32 HwiPrior;
typedef U32 HwiMode;
typedef void (*HwiProcFunc)(HwiArg);

#define OS_HWI_MAX_NUM 256U
#define OS_IRQ2HWI(irq) ((irq) & 0xFFU)
#define OS_HWI_NUM_CHECK(hwiNum) ((hwiNum) >= OS_HWI_MAX_NUM)
#define OS_HWI_MODE_INV(hwiNum) (0)

struct TagHwiHandleForm {
    void (*hwiHandler)(HwiArg);
    HwiArg hwiPram;
};

struct TagHwiModeForm {
    U16 prior;
    U16 mode;
};

#define TRUE 1
#define FALSE 0
#define OS_OK 0U
#define EOK 0
#define OS_SEC_L4_TEXT

extern U32 g_defHandlerHwiNum;
extern U32 g_disable_count;
extern unsigned char g_disabled_mask[OS_HWI_MAX_NUM];

void OsHwiDefaultHandler(HwiArg arg);
HwiProcFunc OsHwiFuncGet(U32 irqNum);
HwiArg OsHwiParaGet(U32 irqNum);
void OsHwiFuncSet(U32 irqNum, HwiProcFunc handler);
void OsHwiParaSet(U32 irqNum, HwiArg arg);
U32 PRT_HwiDisable(HwiHandle hwiNum);
void OsHwiDescInitAll(void);

static void print_case_result(const char *name, long long expected, long long actual, int passed)
{
    printf("[CASE] %s\n  Expected: %lld\n  Actual: %lld\n  Result: %s\n", name, expected, actual, passed ? "PASS" : "FAIL");
}

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

#endif /* TEST_SUPPORT_H */
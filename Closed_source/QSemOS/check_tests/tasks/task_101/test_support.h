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
#define OS_SEC_ALW_INLINE static
#define INLINE inline

extern U32 g_defHandlerHwiNum;
void OsHwiDefaultHandler(HwiArg arg);
HwiProcFunc OsHwiFuncGet(U32 irqNum);
HwiArg OsHwiParaGet(U32 irqNum);
void OsHwiFuncSet(U32 irqNum, HwiProcFunc handler);
void OsHwiParaSet(U32 irqNum, HwiArg arg);

static void print_case_result(const char *name, long long expected, long long actual, int passed)
{
    printf("[CASE] %s\n  Expected: %lld\n  Actual: %lld\n  Result: %s\n", name, expected, actual, passed ? "PASS" : "FAIL");
}

#endif

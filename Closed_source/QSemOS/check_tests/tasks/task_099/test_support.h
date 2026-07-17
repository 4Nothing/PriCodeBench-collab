#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef uint32_t U32;
typedef uint16_t U16;

#define OS_HWI_MAX_NUM 64U
#define OS_OK 0U
#define OS_SEC_L4_TEXT
#define TRUE true
#define FALSE false
#define OS_HWI_SPINLOCK_INIT()
#define OS_IRQ2HWI(irq) (irq)

typedef U32 HwiArg;
typedef U32 HwiHandle;
typedef U16 HwiPrior;
typedef U16 HwiMode;
typedef void (*HwiProcFunc)(HwiArg);

struct TagHwiHandleForm {
    HwiProcFunc hwiHandler;
    HwiArg hwiPram;
};

struct TagHwiModeForm {
    U16 prior;
    U16 mode;
};

void OsHwiDefaultHandler(HwiArg arg);
HwiProcFunc OsHwiFuncGet(U32 irqNum);

#include "auto_stub.h"

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

void print_case_result(const char *name, long long expect, long long actual, int passed);

#endif /* TEST_SUPPORT_H */
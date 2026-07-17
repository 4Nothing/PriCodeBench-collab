#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef uint32_t U32;
typedef uint16_t U16;
typedef uintptr_t HwiArg;
typedef U32 HwiHandle;
typedef U32 HwiPrior;
typedef U32 HwiMode;

#define OS_OK 0
#define OS_ERRNO_HWI_UNCREATED 0x20000006
#define OS_HWI_MAX_NUM 128
#define EOK 0
#define OS_SEC_L4_TEXT

#define TRUE 1
#define FALSE 0

#define OS_IRQ2HWI(irq) ((irq) + 16)

extern long long g_last_error_reported;

struct TagHwiHandleForm {
    void (*hwiHandler)(HwiArg);
    HwiArg hwiPram;
};

struct TagHwiModeForm {
    U16 prior;
    U16 mode;
};

static inline void print_case_result(const char *name, long long expected, long long actual, int passed)
{
    printf("[CASE] %s\n", name);
    printf("  Expected: %lld\n", expected);
    printf("  Actual  : %lld\n", actual);
    printf("  Result  : %s\n\n", passed ? "PASS" : "FAIL");
}

#endif /* TEST_SUPPORT_H */

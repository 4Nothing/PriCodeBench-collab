#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include <stdio.h>
#include <stdint.h>

typedef uint32_t U32;
typedef uint16_t U16;
typedef uintptr_t HwiArg;
typedef U32 HwiHandle;
typedef U16 HwiPrior;
typedef U16 HwiMode;
typedef void (*HwiProcFunc)(HwiArg);

#define OS_OK 0
#define OS_SEC_L4_TEXT
#define OS_SEC_ALW_INLINE static
#define INLINE inline
#define OS_SEC_TEXT
#define OS_HWI_MODE_INV(irqNum) ((irqNum) >= OS_HWI_MAX_NUM)
#define OS_ERRNO_HWI_PROC_FUNC_NULL 0x02000802U
#define OS_ERRNO_HWI_MODE_UNSET 0x02000804U
#define OS_HWI_MAX_NUM 32
#define OS_IRQ2HWI(irq) ((irq) + 16)
#define OS_HWI_GET_HWINUM(hwi) ((hwi) & 0xFF)
#define OS_HWI2IRQ(hwi) ((hwi) - 16)
#define TRUE 1
#define FALSE 0

#define OS_HWI_NUM_CHECK(hwiNum) ((hwiNum) >= OS_HWI_MAX_NUM)
#define OS_HWI_IRQ_LOCK(intSave) ((void)intSave)
#define OS_HWI_IRQ_UNLOCK(intSave) ((void)intSave)
#define OS_HWI_MODE_INV(irqNum) ((irqNum) >= OS_HWI_MAX_NUM)
#define OS_MHOOK_ACTIVATE_PARA1(hook, para) do { (void)hook; (void)para; } while(0)

#define OS_ERRNO_HWI_ALREADY_CREATED 0x02000803U

struct TagHwiHandleForm {
    HwiProcFunc hwiHandler;
    HwiArg hwiPram;
};

struct TagHwiModeForm {
    U16 prior;
    U16 mode;
};

extern U32 g_disable_count;
extern void OsHwiDefaultHandler(HwiArg arg);
extern void OsHwiFuncSet(U32 irqNum, HwiProcFunc handler);
extern void OsHwiParaSet(U32 irqNum, HwiArg arg);
extern void OsHwiHandleActive(U32 irqNum);
extern U32 PRT_HwiDisable(HwiHandle hwiNum);
extern HwiProcFunc OsHwiFuncGet(U32 irqNum);

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

static void print_case_result(const char *name, long long expected, long long actual, int passed)
{
    printf("[CASE] %s\n  Expected: %lld\n  Actual: %lld\n  Result: %s\n", 
           name, expected, actual, passed ? "PASS" : "FAIL");
}

#endif /* TEST_SUPPORT_H */

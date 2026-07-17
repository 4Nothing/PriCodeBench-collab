#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

// Under-test function prototype (implemented in the real source, reimplemented minimally here for unit isolation)
#include "auto_stub.h"
#include "func_under_test.c"


static int total = 0, passed = 0;

static void reset_globals(void)
{
    UNI_FLAG = 0;
    memset(&g_tskModInfo, 0, sizeof(g_tskModInfo));
    g_tskMaxNum = 0;
    g_timeSliceCycle = 0;
    g_systemClock = 1000000u; // 1 MHz
    g_tskIdleEntry = NULL;
}

static void case_maxNum_zero(void)
{
    reset_globals();
    struct TskModInfo m = {0};
    m.maxNum = 0;
    U32 r = OsTskRegister(&m);
    total += 1;
    passed += run_check("maxNum==0 => NOT_SUITED", OS_ERRNO_TSK_MAX_NUM_NOT_SUITED, r);
}

static void case_maxNum_too_big(void)
{
    reset_globals();
    struct TskModInfo m = {0};
    m.maxNum = MAX_TASK_NUM + 1;
    U32 r = OsTskRegister(&m);
    total++;
    passed += run_check("maxNum>MAX_TASK_NUM => NOT_SUITED", OS_ERRNO_TSK_MAX_NUM_NOT_SUITED, r);
}

static void case_timeSlice_default_when_zero(void)
{
    reset_globals();
    struct TskModInfo m = {.maxNum = 10, .timeSliceMs = 0, .defaultSize = 128, .idleStackSize = 64, .magicWord = 0xAA55};
    U32 r = OsTskRegister(&m);
    total++;
    passed += run_check("return OS_OK", OS_OK, r);
    total++;
    passed += run_check("timeSliceMs default", OS_TSK_DEFAULT_TIME_SLICE_MS, g_tskModInfo.timeSliceMs);
}

static void case_timeSlice_custom(void)
{
    reset_globals();
    struct TskModInfo m = {.maxNum = 20, .timeSliceMs = 7, .defaultSize = 256, .idleStackSize = 128, .magicWord = 0x1234};
    U32 r = OsTskRegister(&m);
    total++;
    passed += run_check("return OS_OK", OS_OK, r);
    total++;
    passed += run_check("timeSliceMs custom", 7, g_tskModInfo.timeSliceMs);
}

static void case_timeSliceCycle_calc(void)
{
    reset_globals();
    g_systemClock = 2000000u; // 2 MHz
    struct TskModInfo m = {.maxNum = 5, .timeSliceMs = 5, .defaultSize = 64, .idleStackSize = 32, .magicWord = 1};
    (void)OsTskRegister(&m);
    // expected cycles = (2_000_000 / 1000) * 5 = 2000 * 5 = 10000
    total++;
    passed += run_check("timeSliceCycle calc", 10000, g_timeSliceCycle);
}

static void case_idle_entry_set_when_null(void)
{
    reset_globals();
    struct TskModInfo m = {.maxNum = 5, .timeSliceMs = 1};
    (void)OsTskRegister(&m);
    total++;
    passed += run_check("idle entry set when NULL", (long long)(uintptr_t)OsTskIdleBgd, (long long)(uintptr_t)g_tskIdleEntry);
}

int main(void)
{
    case_maxNum_zero();
    case_maxNum_too_big();
    case_timeSlice_default_when_zero();
    case_timeSlice_custom();
    case_timeSliceCycle_calc();
    case_idle_entry_set_when_null();

    double passRate = (total == 0) ? 0.0 : (100.0 * (double)passed / (double)total);
    printf("Pass-Rate: %.2f%%\n", passRate);
    return (passed == total) ? 0 : 1;
}

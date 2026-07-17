#include "test_utils.h"
#include "prt_perf_output.h"
#include <stdio.h>
#include <string.h>
#include "func_under_test.c"


// Count notify invocations
static int notify_count = 0;
static void notify_hook(void) { notify_count++; }

int main(void)
{
    TestStats stats;
    tests_init(&stats);

    // Init with size 16 and set watermark to 16/PERF_BUFFER_WATERMARK_ONE_N
    OsPerfOutPutInit(NULL, 16);
    OsPerfNotifyHookReg(NULL); // start with none

    // 1) write 0 bytes
    U32 r1 = OsPerfOutPutWrite("", 0);
    char a1[64];
    snprintf(a1, sizeof(a1), "ret=%u remain=%u", r1, OsPerfOutPutRemainSize());
    tests_print_case("Write 0 bytes", "ret=0 remain=16", a1, (r1 == OS_OK && OsPerfOutPutRemainSize() == 16), &stats);

    // 2) write 5 bytes
    U32 r2 = OsPerfOutPutWrite("hello", 5);
    char a2[64];
    snprintf(a2, sizeof(a2), "ret=%u remain=%u", r2, OsPerfOutPutRemainSize());
    tests_print_case("Write 5 bytes", "ret=0 remain=11", a2, (r2 == OS_OK && OsPerfOutPutRemainSize() == 11), &stats);

    // 3) write 11 bytes exact fit
    U32 r3 = OsPerfOutPutWrite("01234567890", 11);
    char a3[64];
    snprintf(a3, sizeof(a3), "ret=%u remain=%u", r3, OsPerfOutPutRemainSize());
    tests_print_case("Write 11 bytes exact", "ret=0 remain=0", a3, (r3 == OS_OK && OsPerfOutPutRemainSize() == 0), &stats);

    // 4) write 1 more (should fail at begin -> OS_ERROR)
    U32 r4 = OsPerfOutPutWrite("Z", 1);
    char a4[64];
    snprintf(a4, sizeof(a4), "ret=%u", r4);
    tests_print_case("Write overflow 1", "ret=1", a4, (r4 == OS_ERROR), &stats);

    // 5) test notify hook when crossing watermark: re-init size 12, watermark=12/4=3
    OsPerfOutPutInit(NULL, 12);
    notify_count = 0;
    OsPerfNotifyHookReg(notify_hook);
    OsPerfOutPutWrite("ab", 2); // used=2 < 3 no notify
    OsPerfOutPutWrite("cd", 2); // used=4 >= 3 notify at least once
    char a5[64];
    snprintf(a5, sizeof(a5), "notify=%d", notify_count);
    tests_print_case("Notify on watermark", "notify=1", a5, (notify_count >= 1), &stats);

    tests_print_summary(&stats);
    return 0;
}

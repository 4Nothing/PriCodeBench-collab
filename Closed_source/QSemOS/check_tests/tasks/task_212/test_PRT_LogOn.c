#include "test_common.h"
#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;
    void *mem = alloc_test_mem();
    (void)PRT_LogInit((uintptr_t)mem);
    (void)PRT_LogSetFilter(OS_LOG_DEBUG);

    // Ensure off then on transitions
    PRT_LogOff();
    long r1 = PRT_Log(OS_LOG_INFO, OS_LOG_F0, "info", 4); // filtered by off -> 0
    print_result("off drops", 0, r1, &pass, &total);

    PRT_LogOn();
    long r2 = PRT_Log(OS_LOG_INFO, OS_LOG_F0, "info", 4); // accepted -> 0
    print_result("on accepts", 0, r2, &pass, &total);

    long r3 = PRT_Log(OS_LOG_EMERG, OS_LOG_F1, "emerg", 5); // always accepted -> 0
    print_result("emerg accepted", 0, r3, &pass, &total);

    long r4 = PRT_Log(OS_LOG_INFO, OS_LOG_F7, NULL, 1); // invalid -> -1
    print_result("invalid arg while on", -1, r4, &pass, &total);

    long r5 = PRT_Log(OS_LOG_INFO, OS_LOG_F7, "", 0); // zero len -> -1
    print_result("zero len while on", -1, r5, &pass, &total);

    finish_report(pass, total);
    free(mem);
    return pass == total ? 0 : 1;
}

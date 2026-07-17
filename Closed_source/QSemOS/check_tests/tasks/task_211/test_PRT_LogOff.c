#include "test_common.h"
#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;
    void *mem = alloc_test_mem();
    (void)PRT_LogInit((uintptr_t)mem);
    (void)PRT_LogSetFilter(OS_LOG_DEBUG);

    PRT_LogOff();
    long r1 = PRT_Log(OS_LOG_INFO, OS_LOG_F0, "info", 4);
    print_result("off filters info", 0, r1, &pass, &total);

    long r2 = PRT_Log(OS_LOG_EMERG, OS_LOG_F1, "emerg", 5); // should still be accepted
    print_result("emerg not filtered by off", 0, r2, &pass, &total);

    PRT_LogOn();
    long r3 = PRT_Log(OS_LOG_INFO, OS_LOG_F2, "info", 4);
    print_result("on accepts info", 0, r3, &pass, &total);

    PRT_LogOff();
    long r4 = PRT_Log((enum OsLogLevel)999, OS_LOG_F0, "x", 1);
    print_result("invalid level returns -1 even while off", -1, r4, &pass, &total);

    long r5 = PRT_Log(OS_LOG_INFO, (enum OsLogFacility)999, "x", 1);
    print_result("invalid fac returns -1 while off", -1, r5, &pass, &total);

    finish_report(pass, total);
    free(mem);
    return pass == total ? 0 : 1;
}

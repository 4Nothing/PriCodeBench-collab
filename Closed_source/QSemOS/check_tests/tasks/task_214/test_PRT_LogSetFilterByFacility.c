#include "test_common.h"
#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;
    void *mem = alloc_test_mem();
    (void)PRT_LogInit((uintptr_t)mem);

    long r;

    // 1. invalid facility
    r = PRT_LogSetFilterByFacility((enum OsLogFacility) - 1, OS_LOG_INFO);
    print_result("invalid facility", -1, r, &pass, &total);

    // 2. invalid level
    r = PRT_LogSetFilterByFacility(OS_LOG_F0, (enum OsLogLevel)999);
    print_result("invalid level", -1, r, &pass, &total);

    // 3. set F0 to OS_LOG_ERR -> info should be filtered (returns 0)
    r = PRT_LogSetFilterByFacility(OS_LOG_F0, OS_LOG_ERR);
    print_result("set F0 to ERR ok", 0, r, &pass, &total);

    r = PRT_Log(OS_LOG_INFO, OS_LOG_F0, "info", 4);
    print_result("info filtered by ERR", 0, r, &pass, &total);

    // 4. EMERG not filtered
    r = PRT_Log(OS_LOG_EMERG, OS_LOG_F0, "emerg", 5);
    print_result("emerg passes regardless", 0, r, &pass, &total);

    // 5. set F0 to DEBUG -> info should pass (0)
    r = PRT_LogSetFilterByFacility(OS_LOG_F0, OS_LOG_DEBUG);
    print_result("set F0 to DEBUG ok", 0, r, &pass, &total);
    r = PRT_Log(OS_LOG_INFO, OS_LOG_F0, "info", 4);
    print_result("info passes with DEBUG", 0, r, &pass, &total);

    finish_report(pass, total);
    free(mem);
    return pass == total ? 0 : 1;
}

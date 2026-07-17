#include "test_common.h"
#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;
    void *mem = alloc_test_mem();
    (void)PRT_LogInit((uintptr_t)mem);

    long r;

    // 1. invalid level lower than EMERG
    r = PRT_LogSetFilter((enum OsLogLevel) - 1);
    print_result("invalid level min", -1, r, &pass, &total);

    // 2. invalid level > NONE
    r = PRT_LogSetFilter((enum OsLogLevel)999);
    print_result("invalid level too big", -1, r, &pass, &total);

    // 3. set to ERR
    r = PRT_LogSetFilter(OS_LOG_ERR);
    print_result("set filter to ERR ok", 0, r, &pass, &total);

    r = PRT_Log(OS_LOG_INFO, OS_LOG_F3, "info", 4);
    print_result("info filtered by ERR", 0, r, &pass, &total);

    // 4. set to DEBUG and test another facility
    r = PRT_LogSetFilter(OS_LOG_DEBUG);
    print_result("set filter to DEBUG ok", 0, r, &pass, &total);

    r = PRT_Log(OS_LOG_INFO, OS_LOG_F7, "info", 4);
    print_result("info passes with DEBUG", 0, r, &pass, &total);

    finish_report(pass, total);
    free(mem);
    return pass == total ? 0 : 1;
}

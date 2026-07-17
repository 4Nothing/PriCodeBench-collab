#include "test_common.h"
#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;
    long r;
    r = OsCheckLog(OS_LOG_EMERG, OS_LOG_F0);
    print_result("min valid", 0, r, &pass, &total);
    r = OsCheckLog(OS_LOG_DEBUG, OS_LOG_F7);
    print_result("max valid", 0, r, &pass, &total);
    r = OsCheckLog((enum OsLogLevel) - 1, OS_LOG_F0);
    print_result("invalid level low", -1, r, &pass, &total);
    r = OsCheckLog(OS_LOG_INFO, (enum OsLogFacility) - 1);
    print_result("invalid facility low", -1, r, &pass, &total);
    r = OsCheckLog((enum OsLogLevel)999, (enum OsLogFacility)999);
    print_result("both invalid high", -1, r, &pass, &total);
    finish_report(pass, total);
    return pass == total ? 0 : 1;
}

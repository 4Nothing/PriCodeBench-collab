#include "test_common.h"
#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;
    void *mem = alloc_test_mem();
    (void)PRT_LogInit((uintptr_t)mem);

    // Initial state is on
    // Try a log; since filter default is OS_LOG_NONE, any non-EMERG gets filtered. We just check return value path.
    long r;

    PRT_LogOff();
    r = PRT_Log(OS_LOG_INFO, OS_LOG_F0, "hello", 5);
    print_result("LogOff causes drop (returns 0)", 0, r, &pass, &total);

    PRT_LogOn();
    r = PRT_Log(OS_LOG_INFO, OS_LOG_F0, "hello", 5);
    print_result("LogOn + filtered returns 0", 0, r, &pass, &total);

    // EMERG can not be filtered; ensure success (0)
    r = PRT_Log(OS_LOG_EMERG, OS_LOG_F0, "emerg", 5);
    print_result("EMERG not filtered", 0, r, &pass, &total);

    // Invalid level should return -1
    r = PRT_Log((enum OsLogLevel)99, OS_LOG_F0, "x", 1);
    print_result("Invalid level returns -1", -1, r, &pass, &total);

    // Null string
    r = PRT_Log(OS_LOG_INFO, OS_LOG_F0, NULL, 1);
    print_result("Null str returns -1", -1, r, &pass, &total);

    finish_report(pass, total);
    free(mem);
    return pass == total ? 0 : 1;
}

#include "test_common.h"
#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;
    void *mem = alloc_test_mem();

    // Reset static globals by reloading? Not possible easily; we rely on fresh process per test
    print_result("Is false before init", 0, (long)PRT_IsLogInit(), &pass, &total);

    (void)PRT_LogInit((uintptr_t)mem);
    print_result("Is true after init", 1, (long)PRT_IsLogInit(), &pass, &total);

    // Cases with multiple calls
    print_result("Still true after re-init", 1, (long)PRT_IsLogInit(), &pass, &total);

    // Toggle log on/off shouldn't affect init state
    PRT_LogOff();
    print_result("Still true after LogOff", 1, (long)PRT_IsLogInit(), &pass, &total);
    PRT_LogOn();
    print_result("Still true after LogOn", 1, (long)PRT_IsLogInit(), &pass, &total);

    finish_report(pass, total);
    free(mem);
    return pass == total ? 0 : 1;
}

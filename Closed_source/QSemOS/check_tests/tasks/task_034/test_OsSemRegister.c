#include "support.h"
#include "prt_sem_external.h"

// Globals required by the module but not defined here
U32 g_maxSem = 0;
struct TagTskCb g_runningTask = {.taskPid = 0};

#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;

    // Case 1: maxNum == 0 -> error
    struct SemModInfo info0 = {.maxNum = 0};
    print_case_u32("OsSemRegister(maxNum=0)", OS_ERRNO_SEM_REG_ERROR, OsSemRegister(&info0), &pass, &total);

    // Case 2: maxNum == 1 -> OK
    struct SemModInfo info1 = {.maxNum = 1};
    print_case_u32("OsSemRegister(maxNum=1)", OS_OK, OsSemRegister(&info1), &pass, &total);

    // Case 3: maxNum == 10 -> OK
    struct SemModInfo info10 = {.maxNum = 10};
    print_case_u32("OsSemRegister(maxNum=10)", OS_OK, OsSemRegister(&info10), &pass, &total);

    // Case 4: maxNum == 100 -> OK
    struct SemModInfo info100 = {.maxNum = 100};
    print_case_u32("OsSemRegister(maxNum=100)", OS_OK, OsSemRegister(&info100), &pass, &total);

    // Case 5: back to zero -> error
    struct SemModInfo info00 = {.maxNum = 0};
    print_case_u32("OsSemRegister(maxNum=0 again)", OS_ERRNO_SEM_REG_ERROR, OsSemRegister(&info00), &pass, &total);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

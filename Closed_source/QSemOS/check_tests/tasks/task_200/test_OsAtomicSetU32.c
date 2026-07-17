#include "test_common.h"
#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;
    U32 v = 0;
    U32 arr[5] = {0, 1, 2, 0x7FFFFFFF, 0xFFFFFFFFu};
    for (int i = 0; i < 5; ++i)
    {
        OsAtomicSetU32(&v, arr[i]);
        char name[64];
        snprintf(name, sizeof(name), "set case %d", i + 1);
        print_result(name, (long)arr[i], (long)v, &pass, &total);
    }
    finish_report(pass, total);
    return pass == total ? 0 : 1;
}

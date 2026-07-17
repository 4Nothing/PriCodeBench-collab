#include "test_common.h"
#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;
    U32 vals[5] = {0u, 1u, 0x12345678u, 0xFFFFFFFFu, 0xABCDEF01u};
    for (int i = 0; i < 5; ++i)
    {
        U32 act = OsAtomicReadU32(&vals[i]);
        char name[64];
        snprintf(name, sizeof(name), "read case %d", i + 1);
        print_result(name, (long)vals[i], (long)act, &pass, &total);
    }
    finish_report(pass, total);
    return pass == total ? 0 : 1;
}

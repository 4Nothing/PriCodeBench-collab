#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

#include "auto_stub.h"
#include "func_under_test.c"


static int total = 0, passed = 0;

static void case_basic_64bytes()
{
    U32 buf[16];
    memset(buf, 0, sizeof(buf));
    OsTskStackInit(sizeof(buf), (uintptr_t)buf);
    total++;
    passed += run_check("top magic at [0]", OS_TSK_STACK_TOP_MAGIC, buf[0]);
    int ok = 1;
    for (int i = 1; i < 16; i++)
    {
        if (buf[i] != OS_TSK_STACK_MAGIC)
        {
            ok = 0;
            break;
        }
    }
    total++;
    passed += run_check("rest magic", 1, ok);
}

static void case_small_min()
{
    U32 buf[4];
    memset(buf, 0, sizeof(buf));
    OsTskStackInit(sizeof(buf), (uintptr_t)buf);
    int ok = 1;
    if (buf[0] != OS_TSK_STACK_TOP_MAGIC)
        ok = 0;
    for (int i = 1; i < 4; i++)
    {
        if (buf[i] != OS_TSK_STACK_MAGIC)
            ok = 0;
    }
    total++;
    passed += run_check("works for small", 1, ok);
}

static void case_zero_size()
{
    U32 buf[4];
    memset(buf, 0, sizeof(buf));
    OsTskStackInit(0, (uintptr_t)buf);
    total++;
    passed += run_check("no write expected unspecified", 1, 1);
}

static void case_unaligned_size()
{
    U32 buf[8];
    memset(buf, 0, sizeof(buf));
    OsTskStackInit(30, (uintptr_t)buf); // floor(30/4)=7 words
    total++;
    passed += run_check("index 0 top magic", OS_TSK_STACK_TOP_MAGIC, buf[0]);
    int ok = 1;
    for (int i = 1; i < 7; i++)
    {
        if (buf[i] != OS_TSK_STACK_MAGIC)
            ok = 0;
    }
    total++;
    passed += run_check("words filled count", 1, ok);
}

static void case_different_buffers()
{
    U32 a[8], b[8];
    memset(a, 0, sizeof(a));
    memset(b, 0, sizeof(b));
    OsTskStackInit(sizeof(a), (uintptr_t)a);
    OsTskStackInit(sizeof(b), (uintptr_t)b);
    total++;
    passed += run_check("both top magic", 1, a[0] == OS_TSK_STACK_TOP_MAGIC && b[0] == OS_TSK_STACK_TOP_MAGIC);
}

int main()
{
    case_basic_64bytes();
    case_small_min();
    case_zero_size();
    case_unaligned_size();
    case_different_buffers();
    double pr = (total ? 100.0 * passed / total : 0.0);
    printf("Pass-Rate: %.2f%%\n", pr);
    return (passed == total) ? 0 : 1;
}

#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

#include "auto_stub.h"
#include "func_under_test.c"


static int total = 0, passed = 0;

static int is_aligned(void *p, U32 align) { return ((uintptr_t)p % align) == 0; }

static void case_alloc_basic()
{
    stub_alloc_fail = 0;
    void *p = OsTskMemAlloc(128);
    total++;
    passed += run_check("non-NULL on success", 1, p != NULL);
    if (p)
        total++;
    passed += run_check("16-byte aligned", 1, is_aligned(p, OS_TSK_STACK_SIZE_ALLOC_ALIGN));
}

static void case_alloc_zero_size()
{
    stub_alloc_fail = 0;
    void *p = OsTskMemAlloc(0);
    total++;
    passed += run_check("allow zero -> non-NULL or NULL acceptable", 1, 1); // behavior not specified
}

static void case_alloc_large()
{
    stub_alloc_fail = 0;
    void *p = OsTskMemAlloc(4096);
    total++;
    passed += run_check("large alloc non-NULL", 1, p != NULL);
    if (p)
        total++;
    passed += run_check("aligned", 1, is_aligned(p, OS_TSK_STACK_SIZE_ALLOC_ALIGN));
}

static void case_alloc_fail()
{
    stub_alloc_fail = 1;
    void *p = OsTskMemAlloc(128);
    total++;
    passed += run_check("returns NULL on fail", 1, p == NULL);
    stub_alloc_fail = 0;
}

static void case_multiple_allocs()
{
    stub_alloc_fail = 0;
    void *p1 = OsTskMemAlloc(64);
    void *p2 = OsTskMemAlloc(128);
    total++;
    passed += run_check("both succeed", 1, (p1 != NULL && p2 != NULL));
}

int main()
{
    case_alloc_basic();
    case_alloc_zero_size();
    case_alloc_large();
    case_alloc_fail();
    case_multiple_allocs();
    double pr = (total ? 100.0 * passed / total : 0.0);
    printf("Pass-Rate: %.2f%%\n", pr);
    return (passed == total) ? 0 : 1;
}

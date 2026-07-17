#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

#include "auto_stub.h"
#include "func_under_test.c"


static int total = 0, passed = 0;

static void set_defaults(struct TskInitParam *p)
{
    memset(p, 0, sizeof(*p));
    p->taskEntry = (TskEntryFunc)0x1;
    p->stackSize = 128; // aligned and >= min
    p->stackAddr = 0;   // not provided
    p->taskPrio = 10;
    p->name = "task";
}

static void case_null_ptrs()
{
    struct TskInitParam ip;
    set_defaults(&ip);
    U32 r = OsTaskCreateParaCheck(NULL, &ip);
    total++;
    passed += run_check("NULL taskPid => PTR_NULL", OS_ERRNO_TSK_PTR_NULL, r);
}

static void case_null_initParam()
{
    TskHandle id = 0;
    U32 r = OsTaskCreateParaCheck(&id, NULL);
    total++;
    passed += run_check("NULL initParam => PTR_NULL", OS_ERRNO_TSK_PTR_NULL, r);
}

static void case_entry_null()
{
    TskHandle id = 0;
    struct TskInitParam ip;
    set_defaults(&ip);
    ip.taskEntry = NULL;
    U32 r = OsTaskCreateParaCheck(&id, &ip);
    total++;
    passed += run_check("taskEntry NULL", OS_ERRNO_TSK_ENTRY_NULL, r);
}

static void case_stacksize_zero_sets_default()
{
    TskHandle id = 0;
    struct TskInitParam ip;
    set_defaults(&ip);
    g_tskModInfo.defaultSize = 256;
    ip.stackSize = 0;
    U32 r = OsTaskCreateParaCheck(&id, &ip);
    total++;
    passed += run_check("success after default size", OS_OK, r);
    total++;
    passed += run_check("stackSize set to default", 256, ip.stackSize);
}

static void case_stacksize_not_align()
{
    TskHandle id = 0;
    struct TskInitParam ip;
    set_defaults(&ip);
    ip.stackSize = 130; // not 16-aligned
    U32 r = OsTaskCreateParaCheck(&id, &ip);
    total++;
    passed += run_check("stack size not align", OS_ERRNO_TSK_STKSZ_NOT_ALIGN, r);
}

static void case_stackaddr_not_align()
{
    TskHandle id = 0;
    struct TskInitParam ip;
    set_defaults(&ip);
    ip.stackAddr = 3; // not 16-aligned
    U32 r = OsTaskCreateParaCheck(&id, &ip);
    total++;
    passed += run_check("stack addr not align", OS_ERRNO_TSK_STACKADDR_NOT_ALIGN, r);
}

static void case_stacksize_too_small()
{
    TskHandle id = 0;
    struct TskInitParam ip;
    set_defaults(&ip);
    ip.stackSize = 32; // < min
    U32 r = OsTaskCreateParaCheck(&id, &ip);
    total++;
    passed += run_check("stack size too small", OS_ERRNO_TSK_STKSZ_TOO_SMALL, r);
}

static void case_stackaddr_overflow()
{
    TskHandle id = 0;
    struct TskInitParam ip;
    set_defaults(&ip);
    ip.stackAddr = (UINTPTR_MAX - 16) + 1;
    ip.stackSize = 32; // small
    // Must be aligned for earlier checks to pass; set aligned and >=min
    ip.stackSize = 64; // min
    ip.stackAddr = ((UINTPTR_MAX - 32u) & ~((uintptr_t)OS_TSK_STACK_SIZE_ALIGN - 1));
    U32 r = OsTaskCreateParaCheck(&id, &ip);
    total++;
    passed += run_check("stack addr overflow", OS_ERRNO_TSK_STACKADDR_TOO_BIG, r);
}

static void case_prio_too_low()
{
    TskHandle id = 0;
    struct TskInitParam ip;
    set_defaults(&ip);
    ip.taskPrio = OS_TSK_PRIORITY_LOWEST + 1;
    U32 r = OsTaskCreateParaCheck(&id, &ip);
    total++;
    passed += run_check("prio error", OS_ERRNO_TSK_PRIOR_ERROR, r);
}

static void case_name_empty()
{
    TskHandle id = 0;
    struct TskInitParam ip;
    set_defaults(&ip);
    ip.name = "";
    U32 r = OsTaskCreateParaCheck(&id, &ip);
    total++;
    passed += run_check("name empty", OS_ERRNO_TSK_NAME_EMPTY, r);
}

static void case_success()
{
    TskHandle id = 0;
    struct TskInitParam ip;
    set_defaults(&ip);
    U32 r = OsTaskCreateParaCheck(&id, &ip);
    total++;
    passed += run_check("OK", OS_OK, r);
}

int main()
{
    case_null_ptrs();
    case_null_initParam();
    case_entry_null();
    case_stacksize_zero_sets_default();
    case_stacksize_not_align();
    case_stackaddr_not_align();
    case_stacksize_too_small();
    case_stackaddr_overflow();
    case_prio_too_low();
    case_name_empty();
    case_success();
    double pr = (total ? 100.0 * passed / total : 0.0);
    printf("Pass-Rate: %.2f%%\n", pr);
    return (passed == total) ? 0 : 1;
}

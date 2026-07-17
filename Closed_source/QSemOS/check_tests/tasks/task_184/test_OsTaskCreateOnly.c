#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

// Forward declarations for mocks used by OsTaskCreateOnly
U32 OsTaskCreateParaCheck(const TskHandle *taskPid, struct TskInitParam *initParam);
U32 OsTaskCreateRsrcInit(U32 taskId, struct TskInitParam *initParam, struct TagTskCb *taskCb, uintptr_t **topStackOut, uintptr_t *curStackSize);
void OsTaskManageFreeCb(struct TagTskCb *taskCB) { (void)taskCB; }
void OsTskStackInit(U32 stackSize, uintptr_t topStack)
{
    (void)stackSize;
    (void)topStack;
}
void OsTskCreateTcbInit(uintptr_t sp, struct TskInitParam *ip, uintptr_t top, uintptr_t size, struct TagTskCb *cb)
{
    (void)sp;
    (void)ip;
    (void)top;
    (void)size;
    (void)cb;
}
void OsTskCreateTcbStatusSet(struct TagTskCb *cb, const struct TskInitParam *ip)
{
    (void)cb;
    (void)ip;
}

// Task entry point - referenced by OsTaskCreateOnly
void OsTskEntry(uintptr_t param1, uintptr_t param2, uintptr_t param3, uintptr_t param4)
{
    (void)param1;
    (void)param2;
    (void)param3;
    (void)param4;
}

#include "auto_stub.h"
#include "func_under_test.c"


static int total = 0, passed = 0;

static U32 para_ok_calls = 0;
static U32 para_ret = OS_OK;
// Override OsTaskCreateParaCheck with wrapper to inject return
U32 OsTaskCreateParaCheck(const TskHandle *taskPid, struct TskInitParam *initParam)
{
    (void)taskPid;
    (void)initParam;
    para_ok_calls++;
    return para_ret;
}

static U32 rsrc_ret = OS_OK;
static uintptr_t *rsrc_top = NULL;
static uintptr_t rsrc_size = 0;
U32 OsTaskCreateRsrcInit(U32 taskId, struct TskInitParam *initParam, struct TagTskCb *taskCb, uintptr_t **topStackOut, uintptr_t *curStackSize)
{
    (void)taskId;
    (void)initParam;
    (void)taskCb;
    *topStackOut = rsrc_top;
    *curStackSize = rsrc_size;
    return rsrc_ret;
}

static void reset()
{
    para_ok_calls = 0;
    para_ret = OS_OK;
    stub_GetTcb_ret = OS_OK;
    stub_GetTcb_ptr = NULL;
    rsrc_ret = OS_OK;
    rsrc_size = 128;
    rsrc_top = (uintptr_t *)malloc(256);
}

static void case_para_check_fail()
{
    reset();
    para_ret = 0xA1u;
    TskHandle id = 0;
    struct TskInitParam ip = {0};
    U32 r = OsTaskCreateOnly(&id, &ip, false);
    total++;
    passed += run_check("propagate para fail", 0xA1, r);
}

static void case_get_tcb_fail()
{
    reset();
    stub_GetTcb_ret = 0xA2u;
    TskHandle id = 0;
    struct TskInitParam ip = {(TskEntryFunc)1, {0}, 1, "t", 0, 0, 128};
    U32 r = OsTaskCreateOnly(&id, &ip, false);
    total++;
    passed += run_check("propagate get tcb fail", 0xA2, r);
}

static void case_rsrc_init_fail()
{
    reset();
    rsrc_ret = 0xA3u;
    TskHandle id = 0;
    struct TskInitParam ip = {(TskEntryFunc)1, {0}, 1, "t", 0, 0, 128};
    U32 r = OsTaskCreateOnly(&id, &ip, false);
    total++;
    passed += run_check("propagate rsrc fail", 0xA3, r);
}

static void case_success_path()
{
    reset();
    TskHandle id = 0;
    struct TskInitParam ip = {(TskEntryFunc)1, {1, 2, 3, 4}, 5, "t", 0, 0, 128};
    U32 r = OsTaskCreateOnly(&id, &ip, false);
    total++;
    passed += run_check("return OK", OS_OK, r);
    total++;
    passed += run_check("task id propagated", 123, id);
}

static void case_sets_status()
{
    reset();
    TskHandle id = 0;
    struct TskInitParam ip = {(TskEntryFunc)1, {0}, 1, "t", 0, 0, 64};
    U32 r = OsTaskCreateOnly(&id, &ip, false);
    total++;
    passed += run_check("OK", OS_OK, r);
}

static void case_is_idle_true()
{
    reset();
    TskHandle id = 0;
    struct TskInitParam ip = {(TskEntryFunc)1, {0}, 1, "t", 0, 0, 64};
    U32 r = OsTaskCreateOnly(&id, &ip, true);
    total++;
    passed += run_check("OK when idle flag", OS_OK, r);
}

int main()
{
    case_para_check_fail();
    case_get_tcb_fail();
    case_rsrc_init_fail();
    case_success_path();
    case_sets_status();
    case_is_idle_true();
    double pr = (total ? 100.0 * passed / total : 0.0);
    printf("Pass-Rate: %.2f%%\n", pr);
    return (passed == total) ? 0 : 1;
}

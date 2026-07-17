#include "test_common.h"
#include "func_under_test.c"

struct DynModuleUnitInfo *g_dynModuleInfoPool[32] = {NULL};

void OsDynModuleUnitFree(struct DynModuleUnitInfo *unitInfo) {
    if (unitInfo != NULL) {
        g_dynModuleInfoPool[unitInfo->unitNo] = NULL;
    }
}

static int run_cases(void)
{
    int pass = 0, total = 0;
    U32 ret;
    struct DynModuleUnitInfo *u[OS_MAX_MODULE_NUM + 1] = {0};

    // 1. allocate one
    total++;
    ret = OsDynModuleUnitAlloc(&u[0]);
    bool ok1 = (ret == OS_MODULE_OK && u[0] != NULL);
    print_case("alloc one", "OK & non-NULL", ok1 ? "OK & non-NULL" : "fail", ok1);
    pass += ok1;

    // 2. allocate up to capacity
    int i;
    int count = ok1 ? 1 : 0;
    bool ok2 = true;
    for (i = count; i < OS_MAX_MODULE_NUM; i++)
    {
        ret = OsDynModuleUnitAlloc(&u[i]);
        if (ret != OS_MODULE_OK || !u[i])
        {
            ok2 = false;
            break;
        }
    }
    total++;
    print_case("fill pool", "all OK", ok2 ? "all OK" : "fail", ok2);
    pass += ok2;

    // 3. next allocation should fail with UNIT_FULL
    total++;
    struct DynModuleUnitInfo *tmp = NULL;
    ret = OsDynModuleUnitAlloc(&tmp);
    bool ok3 = (ret == OS_MODULE_ERRNO_UNIT_FULL);
    print_case("overflow", "UNIT_FULL", ok3 ? "UNIT_FULL" : "other", ok3);
    pass += ok3;

    // 4. free one and allocate again
    total++;
    if (u[0])
        OsDynModuleUnitFree(u[0]), u[0] = NULL;
    ret = OsDynModuleUnitAlloc(&u[0]);
    bool ok4 = (ret == OS_MODULE_OK && u[0] != NULL);
    print_case("free+alloc", "OK", ok4 ? "OK" : "fail", ok4);
    pass += ok4;

    // 5. cleanup no crash
    total++;
    for (i = 0; i < OS_MAX_MODULE_NUM; i++)
        if (u[i])
            OsDynModuleUnitFree(u[i]);
    print_case("cleanup", "no crash", "no crash", true);
    pass += 1;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_cases(); }

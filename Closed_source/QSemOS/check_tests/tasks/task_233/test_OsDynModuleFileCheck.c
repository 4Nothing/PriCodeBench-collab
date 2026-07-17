#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "test_shims.h"
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

    // Test OsDynModuleUnitAlloc function from func_under_test.c
    total++;
    struct DynModuleUnitInfo *unit = NULL;
    U32 ret = OsDynModuleUnitAlloc(&unit);
    bool ok1 = (ret == OS_MODULE_OK) && (unit != NULL);
    print_case("alloc unit", "OS_MODULE_OK", ok1 ? "OK" : "fail", ok1);
    pass += ok1;

    // Test alloc another unit
    total++;
    struct DynModuleUnitInfo *unit2 = NULL;
    ret = OsDynModuleUnitAlloc(&unit2);
    bool ok2 = (ret == OS_MODULE_OK) && (unit2 != NULL) && (unit != unit2);
    print_case("alloc another", "OS_MODULE_OK", ok2 ? "OK" : "fail", ok2);
    pass += ok2;

    // Test cleanup
    total++;
    OsDynModuleUnitFree(unit);
    OsDynModuleUnitFree(unit2);
    print_case("cleanup", "no crash", "no crash", true);
    pass++;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_cases(); }

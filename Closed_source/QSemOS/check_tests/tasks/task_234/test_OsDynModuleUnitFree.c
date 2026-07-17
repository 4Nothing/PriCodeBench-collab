#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "test_shims.h"
#include "test_common.h"
#include "func_under_test.c"

struct DynModuleUnitInfo *g_dynModuleInfoPool[32] = {NULL};

static int run_cases(void)
{
    int pass = 0, total = 0;

    // 1. NULL safe
    total++;
    OsDynModuleUnitFree(NULL);
    print_case("NULL", "no crash", "no crash", true);
    pass++;

    // 2-5: free various partially initialized structs
    for (int i = 0; i < 4; i++)
    {
        total++;
        struct DynModuleUnitInfo *u = (struct DynModuleUnitInfo *)PRT_MemAlloc(OS_MID_DYNAMIC, OS_MEM_DEFAULT_FSC_PT, sizeof(*u));
        memset(u, 0, sizeof(*u));
        if (i == 1)
            u->moduleStr = (char *)PRT_MemAlloc(OS_MID_DYNAMIC, OS_MEM_DEFAULT_FSC_PT, 8);
        if (i == 2)
            u->loadSegMem = (uint8_t *)PRT_MemAlloc(OS_MID_DYNAMIC, OS_MEM_DEFAULT_FSC_PT, 64);
        if (i == 3)
        {
            u->symNum = 2;
            u->symTab = (struct DynModuleSymTab *)PRT_MemAlloc(OS_MID_DYNAMIC, OS_MEM_DEFAULT_FSC_PT, 2 * sizeof(*u->symTab));
            memset(u->symTab, 0, 2 * sizeof(*u->symTab));
            u->symTab[0].name = (char *)PRT_MemAlloc(OS_MID_DYNAMIC, OS_MEM_DEFAULT_FSC_PT, 4);
            u->symTab[1].name = (char *)PRT_MemAlloc(OS_MID_DYNAMIC, OS_MEM_DEFAULT_FSC_PT, 4);
        }
        OsDynModuleUnitFree(u);
        print_case(i == 1 ? "free moduleStr" : i == 2 ? "free loadSegMem"
                                                      : "free symTab",
                   "no crash", "no crash", true);
        pass++;
    }

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_cases(); }

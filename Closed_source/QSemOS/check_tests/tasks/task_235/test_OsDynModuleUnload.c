#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "test_shims.h"
#include "test_common.h"
#include "func_under_test.c"

extern void OsDynModuleUnload(void *handle);

static int run_cases(void)
{
    int pass = 0, total = 0;

    // 1. NULL handle should be safe
    total++;
    OsDynModuleUnload(NULL);
    print_case("NULL handle", "no crash", "no crash", true);
    pass++;

    // 2-5: Call multiple times with NULL, ensure idempotent and no crash
    total++;
    OsDynModuleUnload(NULL);
    print_case("NULL handle again", "no crash", "no crash", true);
    pass++;

    total++;
    OsDynModuleUnload(NULL);
    print_case("NULL handle third", "no crash", "no crash", true);
    pass++;

    total++;
    OsDynModuleUnload(NULL);
    print_case("NULL handle fourth", "no crash", "no crash", true);
    pass++;

    total++;
    OsDynModuleUnload(NULL);
    print_case("NULL handle fifth", "no crash", "no crash", true);
    pass++;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void)
{
    return run_cases();
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include "test_common.h"

char *g_dynModuleErrorStr = NULL;

const char *OsDynModuleGetError(void) {
    return g_dynModuleErrorStr;
}

#include "func_under_test.c"

static int run_cases(void)
{
    int pass = 0, total = 0;

    // Test OsDynModuleSetError function from func_under_test.c
    total++;
    OsDynModuleSetError("test error %d", 123);
    const char *err = OsDynModuleGetError();
    bool ok1 = (err != NULL) && (strstr(err, "test error") != NULL);
    print_case("set error", "error set", ok1 ? "set" : "not set", ok1);
    pass += ok1;

    // Test with NULL format
    total++;
    g_dynModuleErrorStr = NULL;
    OsDynModuleSetError(NULL, 123);
    bool ok2 = (g_dynModuleErrorStr == NULL);
    print_case("null format", "no crash", ok2 ? "ok" : "fail", ok2);
    pass += ok2;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_cases(); }

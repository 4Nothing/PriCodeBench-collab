#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include "test_common.h"

char *g_dynModuleErrorStr = NULL;

const char *OsDynModuleGetError(void) {
    return g_dynModuleErrorStr;
}

#include "func_under_test.c"

static int run_cases(void)
{
    int pass = 0, total = 0;

    // 1. simple string
    total++;
    OsDynModuleSetError("hello");
    bool ok1 = (OsDynModuleGetError() != NULL) && strcmp(OsDynModuleGetError(), "hello") == 0;
    print_case("simple", "hello", OsDynModuleGetError() ? OsDynModuleGetError() : "(null)", ok1);
    pass += ok1;

    // 2. formatted integer
    total++;
    OsDynModuleSetError("num %d", 42);
    bool ok2 = OsDynModuleGetError() && strcmp(OsDynModuleGetError(), "num 42") == 0;
    print_case("format int", "num 42", OsDynModuleGetError() ? OsDynModuleGetError() : "(null)", ok2);
    pass += ok2;

    // 3. long string truncation safety (should not crash)
    total++;
    char big[600];
    memset(big, 'A', sizeof(big));
    big[sizeof(big) - 1] = '\0';
    OsDynModuleSetError("%s", big);
    bool ok3 = OsDynModuleGetError() != NULL; // content may be truncated by shim
    print_case("long string", "non-NULL", ok3 ? "non-NULL" : "NULL", ok3);
    pass += ok3;

    // 4. multiple calls override
    total++;
    OsDynModuleSetError("first");
    OsDynModuleSetError("second");
    bool ok4 = OsDynModuleGetError() && strcmp(OsDynModuleGetError(), "second") == 0;
    print_case("override", "second", OsDynModuleGetError() ? OsDynModuleGetError() : "(null)", ok4);
    pass += ok4;

    // 5. empty format string
    total++;
    OsDynModuleSetError("");
    bool ok5 = OsDynModuleGetError() != NULL && strcmp(OsDynModuleGetError(), "") == 0;
    print_case("empty", "", OsDynModuleGetError() ? OsDynModuleGetError() : "(null)", ok5);
    pass += ok5;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_cases(); }

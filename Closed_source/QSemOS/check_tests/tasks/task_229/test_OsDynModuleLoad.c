#include "support.h"
#include "prt_dynmodule_external.h"

#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;

    // Case 1: NULL file path -> error
    void *r1 = OsDynModuleLoad(NULL, 0);
    print_case_bool("NULL path returns NULL", true, (r1 == NULL), &pass, &total);

    // Case 2: Non-existent file -> error
    void *r2 = OsDynModuleLoad("/nonexistent/path/module.so", 0);
    print_case_bool("Invalid path returns NULL", true, (r2 == NULL), &pass, &total);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "test_common.h"
#include "func_under_test.c"

extern void *OsDynModuleFind(void *handle, const char *symbol);

static void dummy1(void) {}
static int dummy2(int x) { return x + 1; }
static char *dummy3(void) { return "ok"; }

static int run_cases(void)
{
    int pass = 0, total = 0;

    struct DynModuleUnitInfo u = {0};
    DynModuleSymTab tab[3];
    tab[0].name = "dummy1";
    tab[0].addr = (void *)dummy1;
    tab[1].name = "dummy2";
    tab[1].addr = (void *)dummy2;
    tab[2].name = "dummy3";
    tab[2].addr = (void *)dummy3;
    u.symTab = tab;
    u.symNum = 3;

    // 1. exact match
    total++;
    void *r1 = OsDynModuleFind(&u, "dummy1");
    bool ok1 = (r1 == (void *)dummy1);
    print_case("exact match", "addr==dummy1", ok1 ? "addr==dummy1" : "addr!=dummy1", ok1);
    pass += ok1;

    // 2. another symbol
    total++;
    void *r2 = OsDynModuleFind(&u, "dummy2");
    bool ok2 = (r2 == (void *)dummy2);
    print_case("second symbol", "addr==dummy2", ok2 ? "addr==dummy2" : "addr!=dummy2", ok2);
    pass += ok2;

    // 3. third symbol
    total++;
    void *r3 = OsDynModuleFind(&u, "dummy3");
    bool ok3 = (r3 == (void *)dummy3);
    print_case("third symbol", "addr==dummy3", ok3 ? "addr==dummy3" : "addr!=dummy3", ok3);
    pass += ok3;

    // 4. not found
    total++;
    void *r4 = OsDynModuleFind(&u, "not_exist");
    bool ok4 = (r4 == NULL);
    print_case("not found", "NULL", ok4 ? "NULL" : "non-NULL", ok4);
    pass += ok4;

    // 5. NULL handle
    total++;
    void *r5 = OsDynModuleFind(NULL, "dummy1");
    bool ok5 = (r5 == NULL);
    print_case("NULL handle", "NULL", ok5 ? "NULL" : "non-NULL", ok5);
    pass += ok5;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void)
{
    return run_cases();
}

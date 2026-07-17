#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

#include "auto_stub.h"
#include "func_under_test.c"


static int total = 0, passed = 0;

static void reset_env() { memset(g_tskCbTable, 0, sizeof(g_tskCbTable)); }

static void case_basic_copy()
{
    reset_env();
    U32 r = OsTaskNameAdd(1, "abc");
    total++;
    passed += run_check("return OK", OS_OK, r);
    total++;
    passed += run_check("name stored", 0, strcmp(g_tskCbTable[1].name, "abc"));
}

static void case_long_name_trunc()
{
    reset_env();
    char longname[128];
    memset(longname, 'x', sizeof(longname));
    longname[127] = '\0';
    U32 r = OsTaskNameAdd(2, longname);
    total++;
    passed += run_check("truncate returns OK", OS_OK, r);
    total++;
    passed += run_check("name truncated to max length", 0, strncmp(g_tskCbTable[2].name, longname, OS_TSK_NAME_LEN - 1));
}

static void case_empty_string()
{
    reset_env();
    U32 r = OsTaskNameAdd(3, "");
    total++;
    passed += run_check("OK for empty name (copies NUL)", OS_OK, r);
    total++;
    passed += run_check("stored empty", 0, strcmp(g_tskCbTable[3].name, ""));
}

static void case_overwrite_previous()
{
    reset_env();
    (void)OsTaskNameAdd(4, "first");
    U32 r = OsTaskNameAdd(4, "second");
    total++;
    passed += run_check("OK overwrite", OS_OK, r);
    total++;
    passed += run_check("updated value", 0, strcmp(g_tskCbTable[4].name, "second"));
}

static void case_multiple_ids()
{
    reset_env();
    (void)OsTaskNameAdd(10, "a");
    (void)OsTaskNameAdd(11, "b");
    total++;
    passed += run_check("id10 name a", 0, strcmp(g_tskCbTable[10].name, "a"));
    total++;
    passed += run_check("id11 name b", 0, strcmp(g_tskCbTable[11].name, "b"));
}

int main()
{
    case_basic_copy();
    case_long_name_trunc();
    case_empty_string();
    case_overwrite_previous();
    case_multiple_ids();
    double pr = (total ? 100.0 * passed / total : 0.0);
    printf("Pass-Rate: %.2f%%\n", pr);
    return (passed == total) ? 0 : 1;
}

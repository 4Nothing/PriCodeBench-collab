#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

#include "auto_stub.h"
#include "func_under_test.c"


static int total = 0, passed = 0;

static void reset_env() { memset(g_tskCbTable, 0, sizeof(g_tskCbTable)); }

static void case_basic()
{
    reset_env();
    strcpy(g_tskCbTable[1].name, "abc");
    char *p = NULL;
    OsTaskNameGet(1, &p);
    total++;
    passed += run_check("points to name", 0, strcmp(p, "abc"));
}

static void case_empty()
{
    reset_env();
    g_tskCbTable[2].name[0] = '\0';
    char *p = NULL;
    OsTaskNameGet(2, &p);
    total++;
    passed += run_check("empty string", 0, strcmp(p, ""));
}

static void case_different_ids()
{
    reset_env();
    strcpy(g_tskCbTable[3].name, "A");
    strcpy(g_tskCbTable[4].name, "B");
    char *p = NULL;
    OsTaskNameGet(3, &p);
    int ok1 = strcmp(p, "A") == 0;
    OsTaskNameGet(4, &p);
    int ok2 = strcmp(p, "B") == 0;
    total++;
    passed += run_check("id 3 ok", 1, ok1);
    total++;
    passed += run_check("id 4 ok", 1, ok2);
}

static void case_pointer_identity()
{
    reset_env();
    strcpy(g_tskCbTable[5].name, "abc");
    char *p = NULL;
    OsTaskNameGet(5, &p);
    total++;
    passed += run_check("same buffer address", (long long)(uintptr_t)g_tskCbTable[5].name, (long long)(uintptr_t)p);
}

static void case_unicode_like()
{
    reset_env();
    strcpy(g_tskCbTable[6].name, "naïve");
    char *p = NULL;
    OsTaskNameGet(6, &p);
    total++;
    passed += run_check("utf8 stored", 0, strcmp(p, "naïve"));
}

int main()
{
    case_basic();
    case_empty();
    case_different_ids();
    case_pointer_identity();
    case_unicode_like();
    double pr = (total ? 100.0 * passed / total : 0.0);
    printf("Pass-Rate: %.2f%%\n", pr);
    return (passed == total) ? 0 : 1;
}

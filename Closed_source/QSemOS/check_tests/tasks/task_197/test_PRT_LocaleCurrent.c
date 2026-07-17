#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

/* PRT_LocaleCurrent now from func_under_test.c */
#include "func_under_test.c"


static int total = 0, passed = 0;

static void reset() { g_running_task = NULL; }

static void case_null_returns_null()
{
    reset();
    locale_t *p = PRT_LocaleCurrent();
    total++;
    passed += run_check("NULL when no running task", 1, p == NULL);
}

static void case_returns_address_of_locale()
{
    reset();
    static struct TagTskCb cb;
    memset(&cb, 0, sizeof(cb));
    cb.locale = (void *)0xABCD;
    g_running_task = &cb;
    locale_t *p = PRT_LocaleCurrent();
    total++;
    passed += run_check("address match", (long long)(uintptr_t)&cb.locale, (long long)(uintptr_t)p);
}

static void case_value_inside_locale()
{
    reset();
    static struct TagTskCb cb;
    memset(&cb, 0, sizeof(cb));
    cb.locale = (void *)0x1234;
    g_running_task = &cb;
    locale_t *p = PRT_LocaleCurrent();
    total++;
    passed += run_check("value 0x1234", (long long)0x1234, (long long)(uintptr_t)(*p));
}

static void case_change_locale_reflects()
{
    reset();
    static struct TagTskCb cb;
    memset(&cb, 0, sizeof(cb));
    cb.locale = (void *)0x1;
    g_running_task = &cb;
    locale_t *p = PRT_LocaleCurrent();
    cb.locale = (void *)0x2;
    total++;
    passed += run_check("pointer same", (long long)(uintptr_t)&cb.locale, (long long)(uintptr_t)p);
    total++;
    passed += run_check("updated value", (long long)0x2, (long long)(uintptr_t)(*p));
}

static void case_multiple_tasks()
{
    reset();
    static struct TagTskCb a, b;
    memset(&a, 0, sizeof(a));
    memset(&b, 0, sizeof(b));
    a.locale = (void *)0xA;
    b.locale = (void *)0xB;
    g_running_task = &a;
    locale_t *pa = PRT_LocaleCurrent();
    g_running_task = &b;
    locale_t *pb = PRT_LocaleCurrent();
    int ok = ((uintptr_t)(*pa) == 0xA) && ((uintptr_t)(*pb) == 0xB);
    total++;
    passed += run_check("separate locales", 1, ok);
}

int main()
{
    case_null_returns_null();
    case_returns_address_of_locale();
    case_value_inside_locale();
    case_change_locale_reflects();
    case_multiple_tasks();
    double pr = (total ? 100.0 * passed / total : 0.0);
    printf("Pass-Rate: %.2f%%\n", pr);
    return (passed == total) ? 0 : 1;
}

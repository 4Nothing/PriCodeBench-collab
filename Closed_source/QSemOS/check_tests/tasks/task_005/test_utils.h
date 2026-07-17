#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        int total;
        int passed;
    } TestStats;

    void tests_init(TestStats *s);
    void tests_print_case(const char *case_name, const char *expected, const char *actual, bool pass, TestStats *s);
    void tests_print_summary(const TestStats *s);

    // stdout capture helpers
    void start_capture(void);
    // Returns a heap-allocated C string with captured stdout (caller must free)
    char *stop_capture(void);

#ifdef __cplusplus
}
#endif

#endif // TEST_UTILS_H

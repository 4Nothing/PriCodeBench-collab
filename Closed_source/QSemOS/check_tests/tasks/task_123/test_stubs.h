#ifndef TEST_STUBS_H
#define TEST_STUBS_H

// Test harness functions provided by test_stubs.c
void test_reset_stats(void);
void EXPECT_INT_EQ(const char *test_name, int expected, int actual);
void test_print_summary(void);

#endif // TEST_STUBS_H
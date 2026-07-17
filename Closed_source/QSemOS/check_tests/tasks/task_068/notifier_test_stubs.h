#ifndef NOTIFIER_TEST_STUBS_H
#define NOTIFIER_TEST_STUBS_H

extern volatile int g_hwi_lock_count;
void print_case_result(const char *name, int expected, int actual, int *passCnt, int *caseIdx);

#endif /* NOTIFIER_TEST_STUBS_H */

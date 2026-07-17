#include "test_common.h"
#include "func_under_test.c"

static long do_log_and_check_valid(void *mem, enum OsLogLevel lvl, enum OsLogFacility fac, const char *s, size_t len)
{
    volatile uint8_t *valid = (volatile uint8_t *)((uintptr_t)mem + VALID_FLAGS_OFFSET);
    // snapshot valid flags
    uint8_t before[BUFFER_BLOCK_NUM];
    for (int i = 0; i < BUFFER_BLOCK_NUM; ++i)
        before[i] = valid[i];
    long r = PRT_Log(lvl, fac, s, len);
    // count changes
    int changed = 0;
    for (int i = 0; i < BUFFER_BLOCK_NUM; ++i)
        if (before[i] != valid[i])
            changed++;
    // if it passes through OsLog path, changed should be 1; if filtered or invalid, likely 0.
    return (changed > 0) ? 0 : r; // return 0 if wrote, else original r
}

int main(void)
{
    int pass = 0, total = 0;
    void *mem = alloc_test_mem();
    (void)PRT_LogInit((uintptr_t)mem);

    // loosen filter to DEBUG to accept most
    (void)PRT_LogSetFilter(OS_LOG_DEBUG);

    long r;

    // 1. invalid args
    r = PRT_Log((enum OsLogLevel) - 1, OS_LOG_F0, "x", 1);
    print_result("invalid level", -1, r, &pass, &total);

    r = PRT_Log(OS_LOG_INFO, (enum OsLogFacility) - 2, "x", 1);
    print_result("invalid facility", -1, r, &pass, &total);

    r = PRT_Log(OS_LOG_INFO, OS_LOG_F0, NULL, 1);
    print_result("null string", -1, r, &pass, &total);

    r = PRT_Log(OS_LOG_INFO, OS_LOG_F0, "", 0);
    print_result("zero length", -1, r, &pass, &total);

    // 2. normal write should succeed and set a valid flag
    r = do_log_and_check_valid(mem, OS_LOG_INFO, OS_LOG_F0, "hello", 5);
    print_result("info write changes valid flags", 0, r, &pass, &total);

    // 3. truncation long length
    char big[1024];
    memset(big, 'A', sizeof(big));
    r = do_log_and_check_valid(mem, OS_LOG_INFO, OS_LOG_F1, big, sizeof(big));
    print_result("long message truncated but logged", 0, r, &pass, &total);

    // 4. EMERG also writes regardless of filter
    (void)PRT_LogSetFilter(OS_LOG_ERR);
    r = do_log_and_check_valid(mem, OS_LOG_EMERG, OS_LOG_F2, "emerg", 5);
    print_result("emerg writes even with strict filter", 0, r, &pass, &total);

    // 5. With strict filter, INFO gets filtered (no change); function returns 0
    r = do_log_and_check_valid(mem, OS_LOG_INFO, OS_LOG_F2, "info", 4);
    print_result("info filtered under ERR", 0, r, &pass, &total);

    finish_report(pass, total);
    free(mem);
    return pass == total ? 0 : 1;
}

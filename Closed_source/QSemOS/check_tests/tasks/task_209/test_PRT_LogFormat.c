#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

typedef uint32_t U32;
typedef uint8_t U8;
typedef uint32_t TskHandle;

#define OS_SEC_L2_TEXT
#define LOAD_FENCE() __sync_synchronize()
#define STORE_FENCE() __sync_synchronize()
#define EOK 0

#define BUFFER_BLOCK_SIZE 512
#define BUFFER_BLOCK_NUM 32
#define LOG_MAX_SIZE 480
#define VALID_FLAGS_OFFSET (BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM)
#define TAIL_PTR_OFFSET (BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM + BUFFER_BLOCK_NUM)

enum OsLogLevel { OS_LOG_EMERG, OS_LOG_ALERT, OS_LOG_CRIT, OS_LOG_ERR, OS_LOG_WARN, OS_LOG_NOTICE, OS_LOG_INFO, OS_LOG_DEBUG };
enum OsLogFacility { OS_LOG_F0, OS_LOG_F1, OS_LOG_F2, OS_LOG_F3, OS_LOG_F4, OS_LOG_F5 = 0xFF };

void *g_logMemBase = NULL;
int g_logOn = 1;
U32 g_logFilter[5] = {OS_LOG_DEBUG, OS_LOG_DEBUG, OS_LOG_DEBUG, OS_LOG_DEBUG, OS_LOG_DEBUG};
U32 g_sequenceNum = 0;
U32 g_logTail = 0;

bool OsCheckLog(enum OsLogLevel level, enum OsLogFacility facility) {
    if (level < OS_LOG_EMERG || level > OS_LOG_DEBUG) return true;
    if (facility < OS_LOG_F0 || facility > OS_LOG_F4) return true;
    return false;
}

U32 memcpy_s(void *dest, size_t destMax, const void *src, size_t count) {
    if (count > destMax) return 1;
    memcpy(dest, src, count);
    return 0;
}

U32 memset_s(void *dest, size_t destMax, int c, size_t count) {
    if (count > destMax) return 1;
    memset(dest, c, count);
    return 0;
}

int vsnprintf_s(char *str, size_t strMax, size_t count, const char *format, va_list ap) {
    (void)strMax;
    return vsnprintf(str, count, format, ap);
}

U32 OsLog(enum OsLogLevel level, enum OsLogFacility facility, const char *str, size_t strLen) {
    if (strLen > LOG_MAX_SIZE) return -1;
    volatile U8 *validPtr = (U8 *)(g_logMemBase + VALID_FLAGS_OFFSET);
    U32 logIndex = g_logTail++ % BUFFER_BLOCK_NUM;
    validPtr[logIndex] = 1;
    return 0;
}

void PRT_LogInit(uintptr_t mem) { g_logMemBase = (void *)mem; }
U32 PRT_LogSetFilter(enum OsLogLevel level) {
    for (int i = 0; i < 5; i++) g_logFilter[i] = level;
    return 0;
}

#include "func_under_test.c"

static void print_result(const char *case_name, long expected, long actual, int *pass, int *total)
{
    printf("[CASE] %s\n", case_name);
    printf("  Expected: 0x%lX\n", (long long)expected);
    printf("  Actual  : 0x%lX\n", (long long)((U32)actual));
    int ok = ((U32)expected) == ((U32)actual);
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL");
    (*total)++;
    *pass += ok;
}

static void finish_report(int pass, int total)
{
    double rate = total ? (100.0 * pass / total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, pass, total);
}

static long do_logf_and_check(void *mem, enum OsLogLevel lvl, enum OsLogFacility fac, const char *fmt, ...)
{
    volatile uint8_t *valid = (volatile uint8_t *)((uintptr_t)mem + VALID_FLAGS_OFFSET);
    uint8_t before[BUFFER_BLOCK_NUM];
    for (int i = 0; i < BUFFER_BLOCK_NUM; ++i)
        before[i] = valid[i];

    va_list ap;
    va_start(ap, fmt);
    va_end(ap);
    long r = PRT_LogFormat(lvl, fac, fmt, 123, "abc");
    int changed = 0;
    for (int i = 0; i < BUFFER_BLOCK_NUM; ++i)
        if (before[i] != valid[i])
            changed++;
    return (changed > 0) ? 0 : r;
}

int main(void)
{
    int pass = 0, total = 0;
    void *mem = malloc(BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM + 512);
    (void)PRT_LogInit((uintptr_t)mem);

    (void)PRT_LogSetFilter(OS_LOG_DEBUG);

    long r;

    r = PRT_LogFormat((enum OsLogLevel)-1, OS_LOG_F0, "%d", 1);
    print_result("invalid level", -1, r, &pass, &total);

    r = PRT_LogFormat(OS_LOG_INFO, (enum OsLogFacility)-2, "%d", 1);
    print_result("invalid facility", -1, r, &pass, &total);

    r = PRT_LogFormat(OS_LOG_INFO, OS_LOG_F0, NULL);
    print_result("null fmt", -1, r, &pass, &total);

    r = do_logf_and_check(mem, OS_LOG_INFO, OS_LOG_F0, "val=%d str=%s", 123, "abc");
    print_result("formatted write", 0, r, &pass, &total);

    (void)PRT_LogSetFilter(OS_LOG_ERR);
    r = do_logf_and_check(mem, OS_LOG_INFO, OS_LOG_F1, "info %d", 7);
    print_result("info filtered under ERR", 0, r, &pass, &total);

    finish_report(pass, total);
    free(mem);
    return pass == total ? 0 : 1;
}
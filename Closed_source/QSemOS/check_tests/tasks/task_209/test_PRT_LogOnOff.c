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
void PRT_LogOn() { g_logOn = 1; }
void PRT_LogOff() { g_logOn = 0; }

#include "func_under_test.c"

static void print_result(const char *case_name, U32 expected, U32 actual, int *pass, int *total)
{
    printf("[CASE] %s\n", case_name);
    printf("  Expected: 0x%08X\n", expected);
    printf("  Actual  : 0x%08X\n", actual);
    int ok = (expected == actual);
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL");
    (*total)++;
    *pass += ok;
}

static void finish_report(int pass, int total)
{
    double rate = total ? (100.0 * pass / total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, pass, total);
}

int main(void)
{
    int pass = 0, total = 0;
    void *mem = malloc(BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM + 512);
    (void)PRT_LogInit((uintptr_t)mem);

    long r;

    PRT_LogOff();
    r = PRT_LogFormat(OS_LOG_INFO, OS_LOG_F0, "hello %d", 5);
    print_result("LogOff causes drop (returns 0)", 0, r, &pass, &total);

    PRT_LogOn();
    r = PRT_LogFormat(OS_LOG_INFO, OS_LOG_F0, "hello %d", 5);
    print_result("LogOn + filtered returns 0", 0, r, &pass, &total);

    r = PRT_LogFormat(OS_LOG_EMERG, OS_LOG_F0, "emerg %d", 5);
    print_result("EMERG not filtered", 0, r, &pass, &total);

    r = PRT_LogFormat((enum OsLogLevel)99, OS_LOG_F0, "x %d", 1);
    print_result("Invalid level returns -1", (U32)-1, r, &pass, &total);

    r = PRT_LogFormat(OS_LOG_INFO, OS_LOG_F0, NULL);
    print_result("Null str returns -1", (U32)-1, r, &pass, &total);

    finish_report(pass, total);
    free(mem);
    return pass == total ? 0 : 1;
}
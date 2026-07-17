#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint32_t U32;
typedef uint8_t U8;
typedef uint32_t TskHandle;

#define OS_SEC_L2_TEXT
#define STORE_FENCE() __sync_synchronize()
#define LOAD_FENCE() __sync_synchronize()
#define EOK 0

#define BUFFER_BLOCK_SIZE 512
#define BUFFER_BLOCK_NUM 32
#define LOG_MAX_SIZE 480
#define VALID_FLAGS_OFFSET (BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM)
#define TAIL_PTR_OFFSET (BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM + BUFFER_BLOCK_NUM)
#define HEAD_PTR_OFFSET (TAIL_PTR_OFFSET + sizeof(U32))

enum OsLogLevel { OS_LOG_EMERG, OS_LOG_ALERT, OS_LOG_CRIT, OS_LOG_ERR, OS_LOG_WARN, OS_LOG_NOTICE, OS_LOG_INFO, OS_LOG_DEBUG, OS_LOG_NONE = 0xFF };
enum OsLogFacility { OS_LOG_F0, OS_LOG_F1, OS_LOG_F2, OS_LOG_F3, OS_LOG_F4 };

struct logHeader {
    U32 sec;
    U32 nanoSec;
    U32 sequenceNum;
    TskHandle taskPid;
    U32 len;
    enum OsLogFacility facility;
    enum OsLogLevel level;
};

void *g_logMemBase = NULL;
U32 g_sequenceNum = 0;
int g_logOn = 1;
enum OsLogLevel g_logFilterLevel = OS_LOG_NONE;
U32 g_logTail = 0;

U32 OsAtomicReadU32(volatile U32 *addr) { return *addr; }

void PRT_TaskSelf(TskHandle *pid) { *pid = 1; }

U32 memcpy_s(void *dest, size_t destMax, const void *src, size_t count) {
    if (count > destMax) return 1;
    memcpy(dest, src, count);
    return 0;
}

void PRT_LogInit(uintptr_t mem) { g_logMemBase = (void *)mem; }
void PRT_LogOn() { g_logOn = 1; }
void PRT_LogOff() { g_logOn = 0; }

static U32 OsLog(enum OsLogLevel level, enum OsLogFacility facility, const char *str, size_t strLen);

static U32 PRT_Log(enum OsLogLevel level, enum OsLogFacility facility, const char *str, size_t strLen) {
    if (!g_logOn || level > g_logFilterLevel) return 0;
    if (level >= OS_LOG_DEBUG + 1 || str == NULL) return -1;
    return OsLog(level, facility, str, strLen);
}

static U32 OsLog(enum OsLogLevel level, enum OsLogFacility facility, const char *str, size_t strLen) {
    U32 currTail, logIndex, sequenceNum, ret;
    U8 validFlag;
    U8 *targetMem;
    struct logHeader header;
    TskHandle taskPid = -1;
    volatile U8 *const validPtr = (U8 *)(g_logMemBase + VALID_FLAGS_OFFSET);

    sequenceNum = g_sequenceNum++;
    currTail = g_logTail++;

    logIndex = currTail % (2 * BUFFER_BLOCK_NUM);
    if (logIndex < BUFFER_BLOCK_NUM) {
        validFlag = 1;
    } else {
        validFlag = 0;
    }
    logIndex = logIndex % (BUFFER_BLOCK_NUM);

    PRT_TaskSelf(&taskPid);
    header.taskPid = taskPid;
    header.len = strLen;
    header.facility = facility;
    header.level = level;

    targetMem = (U8 *)g_logMemBase + (logIndex * BUFFER_BLOCK_SIZE);
    (void)memcpy_s(targetMem, sizeof(struct logHeader), &header, sizeof(struct logHeader));
    ret = memcpy_s(targetMem + sizeof(struct logHeader), LOG_MAX_SIZE, str, strLen);

    *(volatile U8 *)(validPtr + logIndex) = validFlag;
    if (ret != EOK) {
        return -1;
    }
    return 0;
}

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
    r = PRT_Log(OS_LOG_INFO, OS_LOG_F0, "hello", 5);
    print_result("LogOff causes drop (returns 0)", 0, r, &pass, &total);

    PRT_LogOn();
    r = PRT_Log(OS_LOG_INFO, OS_LOG_F0, "hello", 5);
    print_result("LogOn + filtered returns 0", 0, r, &pass, &total);

    r = PRT_Log(OS_LOG_EMERG, OS_LOG_F0, "emerg", 5);
    print_result("EMERG not filtered", 0, r, &pass, &total);

    r = PRT_Log((enum OsLogLevel)99, OS_LOG_F0, "x", 1);
    print_result("Invalid level returns -1", -1, r, &pass, &total);

    r = PRT_Log(OS_LOG_INFO, OS_LOG_F0, NULL, 1);
    print_result("Null str returns -1", -1, r, &pass, &total);

    finish_report(pass, total);
    free(mem);
    return pass == total ? 0 : 1;
}
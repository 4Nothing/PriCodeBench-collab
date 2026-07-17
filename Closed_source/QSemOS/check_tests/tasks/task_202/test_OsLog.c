#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

typedef uint32_t U32;
typedef uint8_t U8;
typedef uint32_t TskHandle;

#define OS_SEC_L2_TEXT
#define STORE_FENCE() __sync_synchronize()
#define EOK 0

#define BUFFER_BLOCK_SIZE 512
#define BUFFER_BLOCK_NUM 32
#define LOG_MAX_SIZE 480
#define VALID_FLAGS_OFFSET (BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM)
#define TAIL_PTR_OFFSET (BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM + BUFFER_BLOCK_NUM)

enum OsLogLevel { OS_LOG_INFO, OS_LOG_WARN, OS_LOG_ERROR };
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
U32 g_logTail = 0;

uintptr_t OsLogLockOn() { return 0; }
void OsLogLockRestore(uintptr_t save) { (void)save; }
U32 OsLogUpdateTail(U32 *tail) {
    *tail = g_logTail++;
    if (g_logMemBase != NULL) {
        volatile uint32_t *tail_ptr = (volatile uint32_t *)((uintptr_t)g_logMemBase + TAIL_PTR_OFFSET);
        *tail_ptr = g_logTail;
    }
    return 0;
}
void PRT_TaskSelf(TskHandle *pid) { *pid = 1; }

U32 memcpy_s(void *dest, size_t destMax, const void *src, size_t count) {
    if (count > destMax) return 1;
    memcpy(dest, src, count);
    return 0;
}

void PRT_LogInit(uintptr_t mem) { g_logMemBase = (void *)mem; }

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
    void *mem = malloc(BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM + 256);
    (void)PRT_LogInit((uintptr_t)mem);

    long r = OsLog(OS_LOG_INFO, OS_LOG_F0, "abc", 3);
    print_result("normal write returns 0", 0, r, &pass, &total);

    char big[BUFFER_BLOCK_SIZE * 2];
    memset(big, 'B', sizeof(big));
    r = OsLog(OS_LOG_INFO, OS_LOG_F1, big, LOG_MAX_SIZE + 1);
    print_result("oversize returns -1", -1, r, &pass, &total);

    volatile uint8_t *valid = (volatile uint8_t *)((uintptr_t)mem + VALID_FLAGS_OFFSET);
    uint8_t beforeArr[BUFFER_BLOCK_NUM];
    for (int i = 0; i < BUFFER_BLOCK_NUM; ++i)
        beforeArr[i] = valid[i];
    (void)OsLog(OS_LOG_INFO, OS_LOG_F2, "x", 1);
    (void)OsLog(OS_LOG_INFO, OS_LOG_F3, "y", 1);
    int changed = 0;
    for (int i = 0; i < BUFFER_BLOCK_NUM; ++i)
        if (beforeArr[i] != valid[i])
        {
            changed = 1;
            break;
        }
    print_result("valid flags update", 1, changed ? 1 : 0, &pass, &total);

    volatile uint32_t *tail_ptr = (volatile uint32_t *)((uintptr_t)mem + TAIL_PTR_OFFSET);
    uint32_t t0 = *tail_ptr;
    (void)OsLog(OS_LOG_INFO, OS_LOG_F4, "z", 1);
    uint32_t t1 = *tail_ptr;
    print_result("tail advanced by 1", 1, (t1 == t0 + 1) ? 1 : 0, &pass, &total);

    volatile uint8_t *base = (volatile uint8_t *)mem;
    struct logHeader *hdr = (struct logHeader *)(base + (0 * BUFFER_BLOCK_SIZE));
    print_result("header len sane", 1, (hdr->len <= LOG_MAX_SIZE) ? 1 : 0, &pass, &total);

    finish_report(pass, total);
    free(mem);
    return pass == total ? 0 : 1;
}
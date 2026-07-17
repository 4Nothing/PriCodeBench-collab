#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef uint32_t U32;
typedef uint32_t uint32_t;

#define OS_SEC_L2_TEXT
#define LOAD_FENCE() __sync_synchronize()
#define STORE_FENCE() __sync_synchronize()

#define BUFFER_BLOCK_SIZE 512
#define BUFFER_BLOCK_NUM 32
#define VALID_FLAGS_OFFSET (BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM)
#define TAIL_PTR_OFFSET (BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM + BUFFER_BLOCK_NUM)
#define HEAD_PTR_OFFSET (TAIL_PTR_OFFSET + sizeof(U32))

struct PrtSpinLock { int lock; };

void *g_logMemBase = NULL;
struct PrtSpinLock g_logLock = {0};

U32 PRT_SplLockInit(struct PrtSpinLock *lock) { (void)lock; return 0; }

bool PRT_IsLogInit(void) { return g_logMemBase != 0; }

#include "func_under_test.c"

static void print_result(const char *case_name, long expected, long actual, int *pass, int *total)
{
    printf("[CASE] %s\n", case_name);
    printf("  Expected: 0x%lX\n", (long long)expected);
    printf("  Actual  : 0x%lX\n", (long long)actual);
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

    print_result("IsLogInit before init", 0, (long)PRT_IsLogInit(), &pass, &total);

    long ret = PRT_LogInit((uintptr_t)mem);
    print_result("LogInit returns 0 first time", 0, ret, &pass, &total);

    print_result("IsLogInit after init", 1, (long)PRT_IsLogInit(), &pass, &total);

    ret = PRT_LogInit((uintptr_t)mem);
    print_result("LogInit re-init returns 0", 0, ret, &pass, &total);

    volatile uint32_t *head_ptr = (volatile uint32_t *)((uintptr_t)mem + HEAD_PTR_OFFSET);
    volatile uint32_t *tail_ptr = (volatile uint32_t *)((uintptr_t)mem + TAIL_PTR_OFFSET);
    uint32_t expect = 0;
    long ok = (*head_ptr == expect && *tail_ptr == expect);
    print_result("Head/Tail initial value", 1, ok ? 1 : 0, &pass, &total);

    finish_report(pass, total);
    free(mem);
    return pass == total ? 0 : 1;
}
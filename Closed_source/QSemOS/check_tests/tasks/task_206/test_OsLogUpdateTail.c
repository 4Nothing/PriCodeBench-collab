#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef uint32_t U32;
#define U32_MAX ((U32)-1)

#define OS_SEC_L2_TEXT
#define M_FENCE() ((void)__sync_synchronize());

#define BUFFER_BLOCK_SIZE 512
#define BUFFER_BLOCK_NUM 32
#define VALID_FLAGS_OFFSET (BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM)
#define TAIL_PTR_OFFSET (BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM + BUFFER_BLOCK_NUM)
#define HEAD_PTR_OFFSET (TAIL_PTR_OFFSET + sizeof(U32))

void *g_logMemBase = NULL;

void OsLogGetTailAndHead(U32 *head, U32 *tail) {
    volatile U32 *const headPtr = (volatile U32 *)((uintptr_t)g_logMemBase + HEAD_PTR_OFFSET);
    volatile U32 *const tailPtr = (volatile U32 *)((uintptr_t)g_logMemBase + TAIL_PTR_OFFSET);
    *head = *headPtr;
    *tail = *tailPtr;
}

uintptr_t OsLogLockOn() { return 0; }
void OsLogLockRestore(uintptr_t value) { (void)value; }

U32 OsAtomicReadU32(volatile U32 *addr) { return *addr; }
void OsAtomicSetU32(volatile U32 *addr, U32 val) { *addr = val; }

void PRT_LogInit(uintptr_t mem) { g_logMemBase = (void *)mem; }

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

int main(void)
{
    int pass = 0, total = 0;
    void *mem = malloc(BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM + 512);
    (void)PRT_LogInit((uintptr_t)mem);

    U32 outTail;
    volatile uint32_t *head_ptr = (volatile uint32_t *)((uintptr_t)mem + HEAD_PTR_OFFSET);
    volatile uint32_t *tail_ptr = (volatile uint32_t *)((uintptr_t)mem + TAIL_PTR_OFFSET);

    *head_ptr = 100;
    *tail_ptr = 100;
    long r = OsLogUpdateTail(&outTail);
    print_result("normal returns 0", 0, r, &pass, &total);
    print_result("outTail equals old tail", 1, (outTail == 100) ? 1 : 0, &pass, &total);
    print_result("mem tail advanced", 1, (*tail_ptr == 101) ? 1 : 0, &pass, &total);

    *head_ptr = 200;
    *tail_ptr = 199;
    r = OsLogUpdateTail(&outTail);
    print_result("tail<head error", -1, r, &pass, &total);

    *head_ptr = 300;
    *tail_ptr = 300 + BUFFER_BLOCK_NUM;
    r = OsLogUpdateTail(&outTail);
    print_result(">=head+N error", -1, r, &pass, &total);

    *head_ptr = U32_MAX - BUFFER_BLOCK_NUM + 1;
    *tail_ptr = *head_ptr;
    r = OsLogUpdateTail(&outTail);
    print_result("wrap ok", 0, r, &pass, &total);

    *head_ptr = 10;
    *tail_ptr = 10;
    r = OsLogUpdateTail(&outTail);
    print_result("advance again ok", 0, r, &pass, &total);

    finish_report(pass, total);
    free(mem);
    return pass == total ? 0 : 1;
}
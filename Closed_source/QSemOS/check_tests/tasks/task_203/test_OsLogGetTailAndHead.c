#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint32_t U32;

#define OS_SEC_L2_TEXT
#define LOAD_FENCE() __sync_synchronize()

#define BUFFER_BLOCK_SIZE 512
#define BUFFER_BLOCK_NUM 32
#define VALID_FLAGS_OFFSET (BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM)
#define TAIL_PTR_OFFSET (BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM + BUFFER_BLOCK_NUM)
#define HEAD_PTR_OFFSET (TAIL_PTR_OFFSET + sizeof(U32))

void *g_logMemBase = NULL;

U32 OsAtomicReadU32(volatile U32 *addr) { return *addr; }

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
    void *mem = malloc(BUFFER_BLOCK_SIZE * BUFFER_BLOCK_NUM + 512);
    (void)PRT_LogInit((uintptr_t)mem);

    U32 head, tail;

    OsLogGetTailAndHead(&head, &tail);
    print_result("initial equal", 1, (head == tail) ? 1 : 0, &pass, &total);

    volatile uint32_t *head_ptr = (volatile uint32_t *)((uintptr_t)mem + HEAD_PTR_OFFSET);
    volatile uint32_t *tail_ptr = (volatile uint32_t *)((uintptr_t)mem + TAIL_PTR_OFFSET);
    *head_ptr = 1000;
    *tail_ptr = 1005;
    OsLogGetTailAndHead(&head, &tail);
    print_result("read custom values", 1, (head == 1000 && tail == 1005) ? 1 : 0, &pass, &total);

    *head_ptr = 0xFFFFFFF0u;
    *tail_ptr = 0xFFFFFFF1u;
    OsLogGetTailAndHead(&head, &tail);
    print_result("near wrap snapshot", 1, (head == 0xFFFFFFF0u && tail == 0xFFFFFFF1u) ? 1 : 0, &pass, &total);

    OsLogGetTailAndHead(&head, &tail);
    print_result("repeat consistent", 1, (head == 0xFFFFFFF0u && tail == 0xFFFFFFF1u) ? 1 : 0, &pass, &total);

    *head_ptr = 0;
    *tail_ptr = 0;
    OsLogGetTailAndHead(&head, &tail);
    print_result("zero values", 1, (head == 0 && tail == 0) ? 1 : 0, &pass, &total);

    finish_report(pass, total);
    free(mem);
    return pass == total ? 0 : 1;
}
#define RINGBUF_STUBS_DISABLED
#include <stdio.h>
#include <string.h>
#include "auto_stub.h"

static inline void OsSpinLockInitInner(uintptr_t *lock) { (void)lock; }
#include "func_under_test.c"

static int equals_u32(const char *name, U32 expected, U32 actual, int *passCnt)
{
    printf("%s -> Expected: %u, Actual: %u\n", name, expected, actual);
    if (expected == actual)
    {
        (*passCnt)++;
        return 1;
    }
    return 0;
}

int main(void)
{
    int pass = 0;
    int total = 0;
    Ringbuf rb;
    char fifo[5];

    /* Case 1: NULL rb */
    total++;
    equals_u32("Case 1 (NULL rb)", OS_ERROR, PRT_RingbufInit(NULL, fifo, sizeof(fifo)), &pass);

    /* Case 2: NULL fifo */
    memset(&rb, 0, sizeof(rb));
    total++;
    equals_u32("Case 2 (NULL fifo)", OS_ERROR, PRT_RingbufInit(&rb, NULL, 4), &pass);

    /* Case 3: size 0 */
    total++;
    equals_u32("Case 3 (size 0)", OS_ERROR, PRT_RingbufInit(&rb, fifo, 0), &pass);

    /* Case 4: valid init */
    total++;
    equals_u32("Case 4 (valid)", OS_OK, PRT_RingbufInit(&rb, fifo, sizeof(fifo)), &pass);

    /* Case 5: double init should fail */
    total++;
    equals_u32("Case 5 (double init)", OS_ERROR, PRT_RingbufInit(&rb, fifo, sizeof(fifo)), &pass);

    double rate = (total == 0) ? 0.0 : (100.0 * pass / total);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}

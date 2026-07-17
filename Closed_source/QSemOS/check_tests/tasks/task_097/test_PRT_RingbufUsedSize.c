#define RINGBUF_STUBS_DISABLED
#include <stdio.h>
#include <string.h>
#include "auto_stub.h"

static inline uintptr_t PRT_SplIrqLock(PrtSpinLock *lock) { (void)lock; return 0; }
static inline void PRT_SplIrqUnlock(PrtSpinLock *lock, uintptr_t flags) { (void)lock; (void)flags; }
static inline void OsSpinLockInitInner(uintptr_t *lock) { (void)lock; }
#include "../task_090/func_under_test.c"
#include "../task_091/func_under_test.c"
#include "../task_092/func_under_test.c"
#include "../task_093/func_under_test.c"
#include "../task_094/func_under_test.c"
#include "../task_098/func_under_test.c"
#include "../task_095/func_under_test.c"
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

    /* Case 1: NULL ringbuf */
    total++;
    equals_u32("Case 1 (NULL)", 0, PRT_RingbufUsedSize(NULL), &pass);

    /* Prepare a buffer and ring */
    char fifo[16];
    memset(fifo, 0, sizeof(fifo));
    Ringbuf rb;
    memset(&rb, 0, sizeof(rb));

    /* Case 2: status not inited */
    total++;
    rb.status = 0;
    rb.fifo = fifo;
    rb.size = sizeof(fifo);
    rb.remain = rb.size;
    rb.startIdx = 0;
    rb.endIdx = 0;
    equals_u32("Case 2 (not inited)", 0, PRT_RingbufUsedSize(&rb), &pass);

    /* Init properly */
    (void)PRT_RingbufInit(&rb, fifo, sizeof(fifo));

    /* Case 3: empty after init */
    total++;
    equals_u32("Case 3 (empty)", 0, PRT_RingbufUsedSize(&rb), &pass);

    /* Case 4: after writing 5 bytes */
    total++;
    char w1[] = {1, 2, 3, 4, 5};
    U32 w = PRT_RingbufWrite(&rb, w1, sizeof(w1));
    (void)w;
    equals_u32("Case 4 (after write 5)", 5, PRT_RingbufUsedSize(&rb), &pass);

    /* Case 5: wrap scenario -> final used size should be 7 */
    total++;
    char w2[] = {6, 7, 8, 9, 10, 11};   /* write 6 more */
    (void)PRT_RingbufWrite(&rb, w2, 6); /* used 11 total -> remain 5 */
    char rbuf[8];
    (void)PRT_RingbufRead(&rb, rbuf, 9); /* read 9 -> used 2, remain 14 */
    char w3[] = {12, 13, 14, 15, 16};
    (void)PRT_RingbufWrite(&rb, w3, 5); /* used becomes 7 */
    equals_u32("Case 5 (after wraps)", 7, PRT_RingbufUsedSize(&rb), &pass);

    double rate = (total == 0) ? 0.0 : (100.0 * pass / total);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}

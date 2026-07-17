#define RINGBUF_STUBS_DISABLED
#include <stdio.h>
#include <string.h>
#include "auto_stub.h"

static inline uintptr_t PRT_SplIrqLock(PrtSpinLock *lock) { (void)lock; return 0; }
static inline void PRT_SplIrqUnlock(PrtSpinLock *lock, uintptr_t flags) { (void)lock; (void)flags; }
static inline void OsSpinLockInitInner(uintptr_t *lock) { (void)lock; }
#include "../task_092/func_under_test.c"
#include "../task_093/func_under_test.c"
#include "../task_094/func_under_test.c"
#include "../task_097/func_under_test.c"
#include "../task_098/func_under_test.c"
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

    /* Case 1: NULL -> no crash; we treat as pass if simply returns */
    total++;
    PRT_RingbufReset(NULL);
    equals_u32("Case 1 (NULL)", 1, 1, &pass);

    /* Prepare rb */
    char fifo[10];
    Ringbuf rb;
    (void)PRT_RingbufInit(&rb, fifo, sizeof(fifo));
    char src[] = {1, 2, 3, 4, 5};
    (void)PRT_RingbufWrite(&rb, src, sizeof(src));

    /* Case 2: before reset used size 5 */
    total++;
    equals_u32("Case 2 (used 5)", 5, PRT_RingbufUsedSize(&rb), &pass);

    /* Reset */
    PRT_RingbufReset(&rb);

    /* Case 3: after reset used 0 */
    total++;
    equals_u32("Case 3 (used 0)", 0, PRT_RingbufUsedSize(&rb), &pass);

    /* Case 4: startIdx == 0 */
    total++;
    equals_u32("Case 4 (startIdx)", 0, rb.startIdx, &pass);

    /* Case 5: endIdx == 0 */
    total++;
    equals_u32("Case 5 (endIdx)", 0, rb.endIdx, &pass);

    double rate = (total == 0) ? 0.0 : (100.0 * pass / total);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}

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
    char storage[10];
    Ringbuf rb;

    /* Case 1: invalid args */
    total++;
    equals_u32("Case 1 (NULL rb)", 0, PRT_RingbufWrite(NULL, (const char *)"a", 1), &pass);

    /* Init */
    (void)PRT_RingbufInit(&rb, storage, sizeof(storage));

    /* Case 2: zero size */
    total++;
    equals_u32("Case 2 (size 0)", 0, PRT_RingbufWrite(&rb, (const char *)"", 0), &pass);

    /* Case 3: write less than capacity */
    total++;
    char data3[] = {1, 2, 3, 4};
    equals_u32("Case 3 (write 4)", 4, PRT_RingbufWrite(&rb, data3, sizeof(data3)), &pass);

    /* Case 4: write remaining exactly */
    total++;
    char data4[] = {5, 6, 7, 8, 9, 10};
    equals_u32("Case 4 (write 6 to fill)", 6, PRT_RingbufWrite(&rb, data4, sizeof(data4)), &pass);

    /* Case 5: write when full returns 0 */
    total++;
    char data5[] = {11};
    equals_u32("Case 5 (full, write 1)", 0, PRT_RingbufWrite(&rb, data5, sizeof(data5)), &pass);

    /* Case 6: after reading, wrap write */
    char tmp[3];
    (void)PRT_RingbufRead(&rb, tmp, 3); /* free 3 */
    total++;
    char data6[] = {12, 13, 14, 15};
    /* can only write 3 */
    equals_u32("Case 6 (wrap write)", 3, PRT_RingbufWrite(&rb, data6, sizeof(data6)), &pass);

    double rate = (total == 0) ? 0.0 : (100.0 * pass / total);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}

#include <stdio.h>
#include "test_helpers.h"
#include "os_stub.h"
#include "func_under_test.c"

static void run_case(const char *name, int expected_true, int actual_true, U32 expected_recur, U32 actual_recur, int *pass, int *total)
{
    (*total)++;
    printf("%s => expected ret=%d recur=%u | actual ret=%d recur=%u\n", name, expected_true, expected_recur, actual_true, actual_recur);
    if (expected_true == actual_true && expected_recur == actual_recur)
        (*pass)++;
}

int main(void)
{
    test_reset();
    int pass = 0, total = 0;

    struct TagSemCb s;

    // 1) BIN RECUR with recurCount>0 => decremented and returns TRUE
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_BIN, SEM_MUTEX_TYPE_RECUR), 0, SEM_MODE_FIFO);
    s.recurCount = 3;
    s.semOwner = RUNNING_TASK->taskPid;
    int ret = OsSemPostIsInvalid(&s);
    run_case("recur decremented", 1, ret, 2, s.recurCount, &pass, &total);

    // 2) BIN and semCount==FULL => TRUE
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_BIN, 0), OS_SEM_FULL, SEM_MODE_FIFO);
    ret = OsSemPostIsInvalid(&s);
    run_case("bin full", 1, ret, 0, s.recurCount, &pass, &total);

    // 3) BIN and semCount==0 => FALSE
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_BIN, 0), 0, SEM_MODE_FIFO);
    ret = OsSemPostIsInvalid(&s);
    run_case("bin not full", 0, ret, 0, s.recurCount, &pass, &total);

    // 4) COUNT type always FALSE
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), 0, SEM_MODE_FIFO);
    ret = OsSemPostIsInvalid(&s);
    run_case("count type", 0, ret, 0, s.recurCount, &pass, &total);

    // 5) BIN RECUR with recurCount==0 behaves as non-recursive
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_BIN, SEM_MUTEX_TYPE_RECUR), 0, SEM_MODE_FIFO);
    s.recurCount = 0;
    ret = OsSemPostIsInvalid(&s);
    run_case("recur zero -> false", 0, ret, 0, s.recurCount, &pass, &total);

    printf("Pass-Rate: %.2f%%\n", total ? (pass * 100.0 / total) : 0.0);
    return pass == total ? 0 : 1;
}

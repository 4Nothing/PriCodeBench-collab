#include <stdio.h>
#include "os_stub.h"
#include "test_helpers.h"
#define OS_OPTION_BIN_SEM 1
#define OS_OPTION_SEM_RECUR_PV 1
#include "func_under_test.c"

static void run_case(const char *name, int expected_true, int actual_true, U32 expected_count, U32 actual_count, int *pass, int *total)
{
    (*total)++;
    printf("%s => expected ret=%d cnt=%u | actual ret=%d cnt=%u\n", name, expected_true, expected_count, actual_true, actual_count);
    if (expected_true == actual_true && expected_count == actual_count)
        (*pass)++;
}

int main(void)
{
    test_reset();
    int pass = 0, total = 0;

    // 1) BIN recursive same owner increments recurCount
    struct TagSemCb s;
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_BIN, SEM_MUTEX_TYPE_RECUR), 0, SEM_MODE_FIFO);
    s.semOwner = RUNNING_TASK->taskPid;
    s.recurCount = 0;
    U32 before = s.recurCount;
    int ret = OsSemPendNotNeedSche(&s, RUNNING_TASK);
    run_case("recursive bin same owner", 1, ret, before + 1, s.recurCount, &pass, &total);

    // 2) Count type, count>0 decremented and owner set
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), 2, SEM_MODE_FIFO);
    ret = OsSemPendNotNeedSche(&s, RUNNING_TASK);
    run_case("count decremented", 1, ret, 1, s.semCount, &pass, &total);

    // 3) Count type zero -> returns false
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), 0, SEM_MODE_FIFO);
    ret = OsSemPendNotNeedSche(&s, RUNNING_TASK);
    run_case("count zero", 0, ret, 0, s.semCount, &pass, &total);

    // 4) BIN type with free count>0 grabs and links bList
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_BIN, 0), 1, SEM_MODE_FIFO);
    ListInit(&RUNNING_TASK->semBList);
    ret = OsSemPendNotNeedSche(&s, RUNNING_TASK);
    int linked = (s.semBList.prev != &s.semBList) || (s.semBList.next != &s.semBList);
    total++;
    printf("bin link bList => expected=1 actual=%d\n", linked);
    if (linked)
        pass++;

    // 5) BIN type with count==0 and not same owner -> false
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_BIN, 0), 0, SEM_MODE_FIFO);
    s.semOwner = 99;
    ret = OsSemPendNotNeedSche(&s, RUNNING_TASK);
    total++;
    printf("bin count zero returns false => expected=1 actual=%d\n", ret == 0);
    if (ret == 0)
        pass++;

    printf("Pass-Rate: %.2f%%\n", total ? (pass * 100.0 / total) : 0.0);
    return pass == total ? 0 : 1;
}

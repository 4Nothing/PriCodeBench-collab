#include "support.h"
#include "prt_sem_external.h"

U32 g_maxSem = 1;
struct TagSemCb semPool[1];
struct TagSemCb *g_allSem = semPool;
struct TagTskCb g_runningTask = {.taskPid = 0};

#include "func_under_test.c"


int main(void)
{
    int pass = 0, total = 0;
    struct TagSemCb cb;
    memset(&cb, 0, sizeof(cb));

    // Case 1: Counting semaphore init
    OsSemCreateCbInit(3, SEM_TYPE_COUNT, SEM_MODE_FIFO, &cb);
    bool ok1 = (cb.semCount == 3) && (cb.semStat == OS_SEM_USED) && (cb.semMode == SEM_MODE_FIFO) &&
               (cb.semType == SEM_TYPE_COUNT) && (cb.semOwner == OS_INVALID_OWNER_ID);
    print_case_bool("Init count sem", true, ok1, &pass, &total);

    // Case 2: Binary semaphore init should init semBList
    OsSemCreateCbInit(1, SEM_TYPE_BIN, SEM_MODE_PRIOR, &cb);
    bool ok2 = cb.semBList.next == &cb.semBList && cb.semBList.prev == &cb.semBList;
    print_case_bool("Init bin list", true, ok2, &pass, &total);

    // Case 3: Recur count reset
    bool ok3 = (cb.recurCount == 0);
    print_case_bool("Recur count reset", true, ok3, &pass, &total);

    // Case 4: semList always initialized
    bool ok4 = cb.semList.next == &cb.semList && cb.semList.prev == &cb.semList;
    print_case_bool("semList initialized", true, ok4, &pass, &total);

    // Case 5: Owner invalid
    print_case_bool("Owner invalid id", true, (cb.semOwner == OS_INVALID_OWNER_ID), &pass, &total);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

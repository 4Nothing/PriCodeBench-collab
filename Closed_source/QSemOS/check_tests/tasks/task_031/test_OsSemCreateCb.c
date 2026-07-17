#include "support.h"
#include "prt_sem_external.h"

U32 g_maxSem = 3;
struct TagSemCb semPool[3];
struct TagSemCb *g_allSem = semPool;
struct TagListObject g_unusedSemList;
struct TagTskCb g_runningTask = {.taskPid = 5};

// Stub for OsSemCreateCbInit (called by func_under_test.c's OsSemCreateCb)
void OsSemCreateCbInit(U32 count, U32 semType, enum SemMode semMode, struct TagSemCb *semCreated);

#include "func_under_test.c"

// Implement OsSemCreateCbInit
void OsSemCreateCbInit(U32 count, U32 semType, enum SemMode semMode, struct TagSemCb *semCreated) {
    semCreated->semCount = count;
    semCreated->semMode = semMode;
    semCreated->semType = semType;
    OS_LIST_INIT(&semCreated->semList);
    OS_LIST_INIT(&semCreated->semBList);
    semCreated->semOwner = OS_INVALID_OWNER_ID;
    semCreated->semStat = OS_SEM_USED;
    semCreated->recurCount = 0;
}

static void reset_free_list(void)
{
    OS_LIST_INIT(&g_unusedSemList);
    for (U32 i = 0; i < g_maxSem; i++)
    {
        g_allSem[i].semId = (U16)i;
        OS_LIST_INIT(&g_allSem[i].semList);
        ListTailAdd(&g_allSem[i].semList, &g_unusedSemList);
    }
}

int main(void)
{
    int pass = 0, total = 0;
    reset_free_list();

    // Case 1: Empty list -> error (first exhaust all semaphores)
    SemHandle h1, h2, h3;
    (void)OsSemCreateCb(1, SEM_TYPE_BIN, SEM_MODE_FIFO, &h1, 0);  // Use sem 0
    (void)OsSemCreateCb(1, SEM_TYPE_BIN, SEM_MODE_FIFO, &h2, 0);  // Use sem 1
    (void)OsSemCreateCb(1, SEM_TYPE_BIN, SEM_MODE_FIFO, &h3, 0);  // Use sem 2
    U32 r1 = OsSemCreateCb(1, SEM_TYPE_BIN, SEM_MODE_FIFO, &h3, 0);  // List empty
    print_case_u32("OsSemCreateCb empty list", OS_ERRNO_SEM_ALL_BUSY, r1, &pass, &total);

    // Case 2: Normal create
    reset_free_list();
    SemHandle h4;
    U32 r2 = OsSemCreateCb(1, SEM_TYPE_COUNT, SEM_MODE_FIFO, &h4, 0);
    print_case_u32("OsSemCreateCb OK", OS_OK, r2, &pass, &total);

    // Case 3: Created semId valid
    struct TagSemCb *c1 = GET_SEM(h4);
    bool ok3 = (c1->semId == 0) && (c1->semCount == 1);
    print_case_bool("semId valid", true, ok3, &pass, &total);

    // Case 4: Create another
    SemHandle h5;
    U32 r4 = OsSemCreateCb(2, SEM_TYPE_BIN, SEM_MODE_PRIOR, &h5, 0);
    print_case_u32("OsSemCreateCb #2 OK", OS_OK, r4, &pass, &total);

    // Case 5: Validate fields
    struct TagSemCb *c2 = GET_SEM(h5);
    bool ok5 = (c2->semCount == 2) && (c2->semMode == SEM_MODE_PRIOR) && (GET_SEM_TYPE(c2->semType) == SEM_TYPE_BIN);
    print_case_bool("Fields correct", true, ok5, &pass, &total);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

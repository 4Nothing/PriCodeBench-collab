#include "support.h"
#include "prt_sem_external.h"

U32 g_maxSem = 3;
struct TagSemCb semPool[3];
struct TagSemCb *g_allSem = semPool;
struct TagListObject g_unusedSemList;
struct TagTskCb g_runningTask = {.taskPid = 5};

// Forward declarations
void OsSemCreateCbInit(U32 count, U32 semType, enum SemMode semMode, struct TagSemCb *semCreated);
U32 OsSemCreateCb(U32 count, U32 semType, enum SemMode semMode, SemHandle *semHandle, U32 cookie);
U32 OsSemCreate(U32 count, U32 semType, enum SemMode semMode, SemHandle *semHandle, U32 cookie);

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

// Implement OsSemCreate (called by PRT_SemCreate)
U32 OsSemCreate(U32 count, U32 semType, enum SemMode semMode, SemHandle *semHandle, U32 cookie) {
    (void)count; (void)semType; (void)semMode; (void)cookie;
    if (semHandle == NULL) return OS_ERRNO_SEM_PTR_NULL;
    if (ListEmpty(&g_unusedSemList)) return OS_ERRNO_SEM_ALL_BUSY;
    struct TagListObject *unusedSem = OS_LIST_FIRST(&g_unusedSemList);
    ListDelete(unusedSem);
    struct TagSemCb *semCreated = GET_SEM_LIST(unusedSem);
    OsSemCreateCbInit(count, semType, semMode, semCreated);
    *semHandle = (SemHandle)semCreated->semId;
    return OS_OK;
}

// Implement OsSemCreateCb (called by PRT_SemCreate via OsSemCreate)
U32 OsSemCreateCb(U32 count, U32 semType, enum SemMode semMode, SemHandle *semHandle, U32 cookie) {
    (void)count; (void)semType; (void)semMode; (void)cookie;
    if (ListEmpty(&g_unusedSemList)) return OS_ERRNO_SEM_ALL_BUSY;
    struct TagListObject *unusedSem = OS_LIST_FIRST(&g_unusedSemList);
    ListDelete(unusedSem);
    struct TagSemCb *semCreated = GET_SEM_LIST(unusedSem);
    OsSemCreateCbInit(count, semType, semMode, semCreated);
    *semHandle = (SemHandle)semCreated->semId;
    return OS_OK;
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

    // Case 1: count > OS_SEM_COUNT_MAX -> error
    U32 r1 = PRT_SemCreate(OS_SEM_COUNT_MAX + 1, (SemHandle *)NULL);
    print_case_u32("PRT_SemCreate overflow", OS_ERRNO_SEM_OVERFLOW, r1, &pass, &total);

    // Case 2: Normal create
    SemHandle h1;
    U32 r2 = PRT_SemCreate(5, &h1);
    print_case_u32("PRT_SemCreate OK", OS_OK, r2, &pass, &total);

    // Case 3: Created is counting sem
    struct TagSemCb *c1 = GET_SEM(h1);
    bool ok3 = (c1->semCount == 5) && (GET_SEM_TYPE(c1->semType) == SEM_TYPE_COUNT);
    print_case_bool("Counting sem created", true, ok3, &pass, &total);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

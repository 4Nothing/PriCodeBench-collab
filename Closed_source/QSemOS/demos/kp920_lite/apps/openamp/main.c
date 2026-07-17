#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "securec.h"
#include "prt_config.h"
#include "cpu_config.h"
#include "prt_config_internal.h"
#include "prt_hwi.h"
#include "prt_task.h"
#include "cpu_config.h"
#ifdef LOSCFG_SHELL_MICA_INPUT
#include "shell.h"
#include "show.h"
#endif

TskHandle g_testTskHandle;
U8 g_memRegion00[OS_MEM_FSC_PT_SIZE];

extern U32 PRT_PrintfInit();
extern U32 PRT_Printf(const char *format, ...);

#if defined(POSIX_TESTCASE) || defined(RHEALSTONE_TESTCASE)
extern U64 g_timerFrequency;
void Init(uintptr_t param1, uintptr_t param2, uintptr_t param3, uintptr_t param4);
#endif

#if defined(OS_OPTION_LINUX) && defined(LINUX_TESTCASE)
void kthreadTest(void);
void schedTest(void);
void waitTest(void);
#endif

#if defined(OS_OPTION_OPENAMP)
extern U32 RpmsgHwiInit(void);
#endif

#if defined(OS_OPTION_OPENAMP) || defined(OS_OPTION_OPENAMP_PROXYBASH)
int TestOpenamp()
{
    int ret;

    ret = rpmsg_service_init();
    if (ret) {
        return ret;
    }
    
    return OS_OK;
}
#endif

#ifdef LOSCFG_SHELL_MICA_INPUT
static int osShellCmdTstReg(int argc, const char **argv)
{
    printf("tstreg: get %d arguments\n", argc);
    for(int i = 0; i < argc; i++) {
        printf("    no %d arguments: %s\n", i + 1, argv[i]);
    }

    return 0;
}

void micaShellInit()
{
    int ret = OsShellInit(0);
    ShellCB *shellCB = OsGetShellCB();
    if (ret != 0 || shellCB == NULL) {
        printf("shell init fail\n");
        return;
    }
    (VOID)memset_s(shellCB->shellBuf, SHOW_MAX_LEN, 0, SHOW_MAX_LEN);
    ret = osCmdReg(CMD_TYPE_EX, "tstreg", XARGS, (CMD_CBK_FUNC)osShellCmdTstReg);
    if (ret == 0) {
        printf("[INFO]: reg cmd 'tstreg' successed!\n");
    } else {
        printf("[INFO]: reg cmd 'tstreg' failed!\n");
    }
}
#endif

void TestTaskEntry()
{
#if defined(OS_OPTION_OPENAMP) || defined(OS_OPTION_OPENAMP_PROXYBASH)
    TestOpenamp();
    while (!is_tty_ready()) {
        PRT_TaskDelay(OS_TICK_PER_SECOND / 10);
    }
#endif

#if defined(SOEM_DEMO) && defined(OS_SUPPORT_SOEM)
    soem_test("eth1");
#endif

#if defined(POSIX_TESTCASE) || defined(RHEALSTONE_TESTCASE)
    printf("TESTCASE TestTaskEntry(Freq:%llu)\r\n", g_timerFrequency);
    Init(0, 0, 0, 0);
#endif
    for (int i = 0; i < 5; i++) {
        printf("TestTaskEntry=============TestTaskEntry\r\n");
    }

#if defined(OS_OPTION_LINUX) && defined(LINUX_TESTCASE)
    kthreadTest();
    schedTest();
    waitTest();
#endif

#ifdef LOSCFG_SHELL_MICA_INPUT
    micaShellInit();
#endif
}

U32 OsTestInit(void)
{
    U32 ret;
    U8 ptNo = OS_MEM_DEFAULT_FSC_PT;
    struct TskInitParam param = {0};
    
    param.stackAddr = (uintptr_t)PRT_MemAllocAlign(0, ptNo, 0x2000, MEM_ADDR_ALIGN_016);
    param.taskEntry = (TskEntryFunc)TestTaskEntry;
    param.taskPrio = 25;
    param.name = "TestTask";
    param.stackSize = 0x2000;

    ret = PRT_TaskCreate(&g_testTskHandle, &param);
    if (ret) {
        return ret;
    }
    
    ret = PRT_TaskResume(g_testTskHandle);
    if (ret) {
        return ret;
    }

    return OS_OK;
}

void Test2TaskEntry()
{
    while (1) {
        printf("task 1. %d\n", OsGetCoreID());
        PRT_TaskDelay(6000);
    }
}

void Test3TaskEntry()
{
    while (1) {
        printf("task 2. %d\n", OsGetCoreID());
        PRT_TaskDelay(4000);
    }
}

U32 TaskTest()
{
    U32 ret;
    U8 ptNo = OS_MEM_DEFAULT_FSC_PT;
    struct TskInitParam param = {0};
    TskHandle testTskHandle[2];

    // task 2
    param.stackAddr = (uintptr_t)PRT_MemAllocAlign(0, ptNo, 0x2000, MEM_ADDR_ALIGN_016);
    param.taskEntry = (TskEntryFunc)Test2TaskEntry;
    param.taskPrio = 20;
    param.name = "Test2Task";
    param.stackSize = 0x2000;

    ret = PRT_TaskCreate(&testTskHandle[0], &param);
    if (ret) {
        return ret;
    }

    ret = PRT_TaskResume(testTskHandle[0]);
    if (ret) {
        return ret;
    }

    // task 3
    param.stackAddr = (uintptr_t)PRT_MemAllocAlign(0, ptNo, 0x2000, MEM_ADDR_ALIGN_016);
    param.taskEntry = (TskEntryFunc)Test3TaskEntry;
    param.taskPrio = 25;
    param.name = "Test3Task";
    param.stackSize = 0x2000;

    ret = PRT_TaskCreate(&testTskHandle[1], &param);
    if (ret) {
        return ret;
    }

    ret = PRT_TaskResume(testTskHandle[1]);
    if (ret) {
        return ret;
    }

    return OS_OK;
}

#if (SMP_TESTCASE)
void SlaveTaskEntry()
{
    printf("slave 1.\n");
    static U32 temp1 = 0;
    while (1) {
        PRT_TaskDelay(5000);
        printf("slave 1. %d\n", OsGetCoreID());
        temp1++;
    }
}

void SlaveTaskEntry2()
{
    static U32 temp2 = 0;
    while (1) {
        printf("slave 2. %d\n", OsGetCoreID());
        PRT_TaskDelay(3000);
        temp2++;
    }
}

U32 SlaveTestInit(U32 slaveId)
{
    U32 ret;
    U8 ptNo = OS_MEM_DEFAULT_FSC_PT;
    struct TskInitParam param = {0};
    TskHandle testTskHandle[2];
    // task 1
    param.stackAddr = PRT_MemAllocAlign(0, ptNo, 0x2000, MEM_ADDR_ALIGN_016);
    param.taskEntry = (TskEntryFunc)SlaveTaskEntry;
    param.taskPrio = 25;
    param.name = "SlaveTask";
    param.stackSize = 0x2000;

    ret = PRT_TaskCreate(&testTskHandle[0], &param);
    if (ret) {
        return ret;
    }

    ret = PRT_TaskCoreBind(testTskHandle[0], 1 << (PRT_GetPrimaryCore() + slaveId));
    if (ret) {
        return ret;
    }

    ret = PRT_TaskResume(testTskHandle[0]);
    if (ret) {
        return ret;
    }
    
    param.stackAddr = PRT_MemAllocAlign(0, ptNo, 0x2000, MEM_ADDR_ALIGN_016);
    param.taskEntry = (TskEntryFunc)SlaveTaskEntry2;
    param.taskPrio = 30;
    param.name = "Test2Task";
    param.stackSize = 0x2000;

    ret = PRT_TaskCreate(&testTskHandle[1], &param);
    if (ret) {
        return ret;
    }

    ret = PRT_TaskCoreBind(testTskHandle[1], 1 << (PRT_GetPrimaryCore() + slaveId));
    if (ret) {
        return ret;
    }

    ret = PRT_TaskResume(testTskHandle[1]);
    if (ret) {
        return ret;
    }

    return OS_OK;
}
#endif

U32 PRT_AppInit(void)
{
    U32 ret;

#if defined(OS_OPTION_OPENAMP)
    ret = RpmsgHwiInit();
    if (ret) {
        return ret;
    }
#endif

#if defined(OS_OPTION_SMP)
#if (SMP_TESTCASE)
    ret = TaskTest();
    if (ret) {
        return ret;
    }

    for (U32 slaveId = 1; slaveId < OS_SYS_CORE_RUN_NUM; slaveId++) {
        ret = SlaveTestInit(slaveId);
        if (ret) {
            return ret;
        }
    }
#endif
#endif

    ret = OsTestInit();
    if (ret) {
        return ret;
    }

    ret = TestClkStart();
    if (ret) {
        return ret;
    }

    return OS_OK;
}

U32 PRT_HardDrvInit(void)
{
    U32 ret;

    ret = OsHwiInit();
    if (ret) {
        return ret;
    }

    return OS_OK;
}

void PRT_HardBootInit(void)
{
}

S32 main(void)
{
    return OsConfigStart();
}

extern void *__wrap_memset(void *dest, int set, U32 len)
{
    if (dest == NULL || len == 0) {
        return NULL;
    }
    
    char *ret = (char *)dest;
    for (int i = 0; i < len; ++i) {
        ret[i] = set;
    }
    return dest;
}

extern void *__wrap_memcpy(void *dest, const void *src, size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        *(char *)(dest + i) = *(char *)(src + i);
    }
    return dest;
}


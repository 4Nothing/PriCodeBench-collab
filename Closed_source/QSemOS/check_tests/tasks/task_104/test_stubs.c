#include "test_support.h"

/* Observable last error */
U32 g_last_error_reported = 0;
U32 g_disable_count = 0;
unsigned char g_disabled_mask[OS_HWI_MAX_NUM] = {0};

/* Simulated IRQ tables (exported for macros) */
static struct TagHwiHandleForm g_hwiForm[OS_HWI_MAX_NUM];
static struct TagHwiModeForm g_hwiModeForm[OS_HWI_MAX_NUM];

/* Default no-op implementations */
void sys_trace_isr_enter(void) {}
void sys_trace_isr_exit(void) {}

/* GIC init just marks called; not used further */
static int g_gic_inited = 0;
void OsHwiGICInit(void) { g_gic_inited = 1; }

U32 g_defHandlerHwiNum;
void OsHwiDefaultHandler(HwiArg arg) { g_defHandlerHwiNum = OS_IRQ2HWI(arg); }

/* Interrupt active dispatcher: call handler with stored arg */
void OsHwiHandleActive(U32 irqNum)
{
    if (irqNum < OS_HWI_MAX_NUM && g_hwiForm[irqNum].hwiHandler)
    {
        g_hwiForm[irqNum].hwiHandler(g_hwiForm[irqNum].hwiPram);
    }
}

/* Form accessors */
void OsHwiFuncSet(U32 irqNum, HwiProcFunc handler)
{
    if (irqNum < OS_HWI_MAX_NUM)
        g_hwiForm[irqNum].hwiHandler = handler;
}

HwiProcFunc OsHwiFuncGet(U32 irqNum)
{
    return (irqNum < OS_HWI_MAX_NUM) ? g_hwiForm[irqNum].hwiHandler : NULL;
}

void OsHwiParaSet(U32 irqNum, HwiArg arg)
{
    if (irqNum < OS_HWI_MAX_NUM)
        g_hwiForm[irqNum].hwiPram = arg;
}

void OsHwiDescFieldInit(U32 irqNum)
{
    OsHwiFuncSet(irqNum, OsHwiDefaultHandler);
    OsHwiParaSet(irqNum, irqNum);
}

void OsHwiDescInitAll(void)
{
    U32 irqNum;
    for (irqNum = 0; irqNum < OS_HWI_MAX_NUM; irqNum++) {
        OsHwiDescFieldInit(irqNum);
    }
}

HwiArg OsHwiParaGet(U32 irqNum)
{
    return (irqNum < OS_HWI_MAX_NUM) ? g_hwiForm[irqNum].hwiPram : 0;
}

U32 OsHwiIsCanCreated(U32 irqNum)
{
    /* Allow if current handler is NULL or Default; tests treat non-NULL and not Default as already created */
    HwiProcFunc h = OsHwiFuncGet(irqNum);
    extern void OsHwiDefaultHandler(HwiArg);
    if (h == NULL || h == OsHwiDefaultHandler)
        return TRUE;
    return FALSE;
}

U32 PRT_HwiDisable(HwiHandle hwiNum)
{
    /* Pretend to disable; always OK */
    if (hwiNum < OS_HWI_MAX_NUM)
        g_disabled_mask[hwiNum] = 1;
    g_disable_count++;
    return OS_OK;
}

U32 OsHwiPrioCheck(HwiPrior prio)
{
    /* Accept priorities 0-255; else error */
    return (prio <= 255u) ? OS_OK : 1u;
}

U32 OsHwiModeCheck(HwiMode mode)
{
    /* Accept mode 0 (normal) or 1 (combine) for tests */
    return (mode <= 1u) ? OS_OK : 1u;
}

void OsHwiAttrSet(U32 irqNum, HwiPrior prio, HwiMode mode)
{
    if (irqNum < OS_HWI_MAX_NUM)
    {
        g_hwiModeForm[irqNum].prior = (U16)prio;
        g_hwiModeForm[irqNum].mode = (U16)mode;
    }
}

U32 OsHwiAttrClear(U32 irqNum)
{
    if (irqNum < OS_HWI_MAX_NUM)
    {
        g_hwiModeForm[irqNum].prior = 0;
        g_hwiModeForm[irqNum].mode = 0;
        return OS_OK;
    }
    return 1u;
}

U32 OsHwiPrioConflictCheck(U32 irqNum, U16 prio)
{
    if (irqNum < OS_HWI_MAX_NUM)
    {
        /* Conflict if attempting to change an already non-zero priority to different value */
        if (g_hwiModeForm[irqNum].prior != 0 && g_hwiModeForm[irqNum].prior != prio)
            return TRUE;
        return FALSE;
    }
    return TRUE;
}

U32 OsHwiNeedPrivateIsr(U32 irqNum)
{
    /* For simple tests, none need private ISR */
    (void)irqNum;
    return FALSE;
}

U16 test_attr_mode_get(U32 irqNum)
{
    return (irqNum < OS_HWI_MAX_NUM) ? g_hwiModeForm[irqNum].mode : 0;
}

U16 test_attr_prior_get(U32 irqNum)
{
    return (irqNum < OS_HWI_MAX_NUM) ? g_hwiModeForm[irqNum].prior : 0;
}

/* Memory helpers */
void *OsMemAllocAlign(int mid, int pt, U32 size, U32 align)
{
    (void)mid;
    (void)pt;
    (void)align;
    (void)align;
    return malloc(size);
}

void *OsMemAlloc(int mid, int pt, U32 size)
{
    (void)mid;
    (void)pt;
    return malloc(size);
}

/* memset_s is defined in auto_stub.h */

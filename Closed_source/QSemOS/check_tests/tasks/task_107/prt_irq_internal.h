#pragma once
#include "auto_stub.h"

/* Declare g_hwiForm and g_hwiModeForm from prt_irq.c */
extern struct TagHwiHandleForm g_hwiForm[OS_HWI_MAX_NUM];
extern struct TagHwiModeForm g_hwiModeForm[OS_HWI_MAX_NUM];

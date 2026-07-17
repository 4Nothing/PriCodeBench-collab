#pragma once
#include "prt_perf.h"

U32 OsPerfOutPutInit(void *buf, U32 size);
U32 OsPerfOutPutWrite(const char *data, U32 len);
U32 OsPerfOutPutRead(char *dest, U32 size);
void OsPerfOutPutFlush(void);
void OsPerfNotifyHookReg(PERF_BUF_NOTIFY_HOOK func);
void OsPerfFlushHookReg(PERF_BUF_FLUSH_HOOK func);

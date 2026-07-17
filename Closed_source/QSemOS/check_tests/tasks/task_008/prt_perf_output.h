#pragma once
#include "auto_stub.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern U32 OsPerfOutPutInit(void *buf, U32 size);
extern U32 OsPerfOutPutRead(char *dest, U32 size);
extern U32 OsPerfOutPutWrite(char *data, U32 size);
extern U32 OsPerfOutPutRemainSize(void);
extern void OsPerfOutPutInfo(void);
extern void OsPerfOutPutFlush(void);

#ifdef __cplusplus
}
#endif

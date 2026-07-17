#pragma once
#include "auto_stub.h"

typedef U64 (*SysTimeHook)(void);
extern SysTimeHook g_sysTimeHook;

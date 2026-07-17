#pragma once
#include "prt_perf.h"

typedef struct Pmu
{
    PerfEvent events;
    U32 type;
    U32 (*config)(void);
    U32 (*start)(void);
    U32 (*stop)(void);
    const char *(*getName)(Event *e);
} Pmu;

// PMU registry mock
Pmu *OsPerfPmuGet(U32 type);

// Optional init functions (weak in tests)
U32 OsHwPmuInit(void);
U32 OsTimedPmuInit(void);
U32 OsSwPmuInit(void);

#pragma once
#include "prt_perf.h"

// PMU registry mock
Pmu *OsPerfPmuGet(U32 type);

// Optional init functions (weak in tests)
U32 OsHwPmuInit(void);
U32 OsTimedPmuInit(void);
U32 OsSwPmuInit(void);

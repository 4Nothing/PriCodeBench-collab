#include "prt_task_external.h"

U32 OS_TASK_LOCK_DATA = 0;
U32 g_int_active = 0;
struct TagTskCb *RUNNING_TASK = NULL;

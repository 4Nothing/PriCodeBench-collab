#include "log_decl.h"
SplLock g_logLock;
uintptr_t PRT_SplIrqLock(SplLock *lock) { (void)lock; return 0; }

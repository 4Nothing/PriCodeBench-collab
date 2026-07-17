#include "log_decl.h"
SplLock g_logLock;
void PRT_SplIrqUnlock(SplLock *lock, uintptr_t value) { (void)lock; (void)value; }

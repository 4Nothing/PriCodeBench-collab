#include <stdint.h>
typedef uintptr_t SplLock;
extern SplLock g_logLock;
void PRT_SplIrqUnlock(SplLock *lock, uintptr_t value);

#include <stdint.h>
typedef uintptr_t SplLock;
extern SplLock g_logLock;
uintptr_t PRT_SplIrqLock(SplLock *lock);

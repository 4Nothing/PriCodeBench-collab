#include <stddef.h>

// Declare external reference to g_taskCoreSleep instead of including header
extern void (*g_taskCoreSleep)(void);

// OsIdleTaskExe - call g_taskCoreSleep if set
void OsIdleTaskExe(void)
{
    if (g_taskCoreSleep != NULL) {
        g_taskCoreSleep();
    }
}
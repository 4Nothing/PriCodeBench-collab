#ifndef MOCK_SECUREC_H
#define MOCK_SECUREC_H

#include <stddef.h>
#include <string.h>

#ifndef EOK
#define EOK 0
#endif

/* Allow tests to inject memset_s failure once by setting this to non-zero. */
static int g_mock_memset_fail = 0;
static inline int memset_s(void *dest, size_t destsz, int c, size_t count)
{
    if (g_mock_memset_fail)
    {
        g_mock_memset_fail = 0; /* one-shot failure */
        return -1;              /* non-EOK */
    }
    /* Best-effort semantics for tests */
    if (dest == NULL || destsz == 0)
    {
        return -1;
    }
    size_t n = count;
    if (n > destsz)
        n = destsz;
    (void)memset(dest, c, n);
    return EOK;
}

#endif /* MOCK_SECUREC_H */

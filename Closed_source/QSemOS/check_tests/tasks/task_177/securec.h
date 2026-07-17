#ifndef MOCK_SECUREC_H
#define MOCK_SECUREC_H

#include <stddef.h>
#include <string.h>

#ifndef EOK
#define EOK 0
#endif

/* memset_s stub - g_mock_memset_fail is defined in notifier_test_stubs.c */
extern int g_mock_memset_fail;

static inline int memset_s(void *dest, size_t destsz, int c, size_t count)
{
    if (dest == NULL || destsz == 0) {
        return -1;
    }
    if (g_mock_memset_fail) {
        g_mock_memset_fail = 0;
        return -1;
    }
    size_t n = count;
    if (n > destsz) {
        n = destsz;
    }
    (void)memset(dest, c, n);
    return EOK;
}

#endif /* MOCK_SECUREC_H */

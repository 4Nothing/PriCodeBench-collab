#ifndef PRT_CACHE_EXTERNAL_H
#define PRT_CACHE_EXTERNAL_H
#include <stddef.h>
#include <stdint.h>
static inline void cache_flush_by_range(void *addr, size_t size)
{
    (void)addr;
    (void)size;
}
#endif

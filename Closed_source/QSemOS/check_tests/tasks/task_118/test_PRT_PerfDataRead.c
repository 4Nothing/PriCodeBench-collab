#include <stddef.h>
void *memset(void *s, int c, size_t n);
void *memcpy(void *restrict dest, const void *restrict src, size_t n);
#include "mock_include/prt_perf.h"
#include "mock_include/prt_perf_output.h"
#include "test_util.h"
#include "func_under_test.c"

static char g_buf[64];
static U32 g_len = 0;
U32 OsPerfOutPutRead(char *dest, U32 size)
{
    U32 n = (g_len < size) ? g_len : size;
    memcpy(dest, g_buf, n);
    return n;
}

int main(void)
{
    TU_reset();
    // simulate buffer content
    memcpy(g_buf, "hello", 5);
    g_len = 5;
    char out[8] = {0};
    U32 n = PRT_PerfDataRead(out, sizeof(out));

    // 1) returned length
    TU_EXPECT_EQ_U32(5, n, "Read length");
    // 2) content first byte
    TU_EXPECT_EQ_U32('h', (U32)out[0], "First byte");
    // 3) content third byte
    TU_EXPECT_EQ_U32('l', (U32)out[2], "Third byte");
    // 4) null-termination not guaranteed but our buffer has zeroes beyond
    TU_EXPECT_EQ_U32(0, (U32)out[5], "Next byte zero");
    // 5) idempotent call with size 0
    U32 m = PRT_PerfDataRead(out, 0);
    TU_EXPECT_EQ_U32(0, m, "Zero-size read returns 0");

    TU_report();
    return 0;
}
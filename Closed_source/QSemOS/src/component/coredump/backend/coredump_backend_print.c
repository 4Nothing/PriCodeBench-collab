/*
 * Copyright (c) 2020 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include "coredump_internal.h"

/* Length of buffer of printable size */
#define LOG_BUF_SZ          64

/* Length of buffer of printable size plus null character */
#define LOG_BUF_SZ_RAW      (LOG_BUF_SZ + 1)

/* Log Buffer */
static char log_buf[LOG_BUF_SZ_RAW];

static int error;

static int hex2char(U8 x, char *c)
{
    if (x <= 9) {
        *c = x + '0';
    } else if (x <= 15) {
        *c = x - 10 + 'a';
    } else {
        return -1;
    }

    return 0;
}

static void coredump_logging_backend_start(void)
{
    PRT_Printf(COREDUMP_PREFIX_STR COREDUMP_BEGIN_STR "\n");
}

static void coredump_logging_backend_end(void)
{
    PRT_Printf(COREDUMP_PREFIX_STR COREDUMP_END_STR "\n");
}

static void coredump_logging_backend_buffer_output(uint8_t *buf, size_t buflen)
{
    uint8_t log_ptr = 0;
    size_t remaining = buflen;
    size_t i = 0;

    if ((buf == NULL) || (buflen == 0)) {
        error = -EINVAL;
        remaining = 0;
    }

    while (remaining > 0) {
        if (hex2char(buf[i] >> 4, &log_buf[log_ptr]) < 0) {
            error = -EINVAL;
            break;
        }
        log_ptr++;

        if (hex2char(buf[i] & 0xf, &log_buf[log_ptr]) < 0) {
            error = -EINVAL;
            break;
        }
        log_ptr++;

        i++;
        remaining--;

        if ((log_ptr >= LOG_BUF_SZ) || (remaining == 0)) {
            log_buf[log_ptr] = '\0';
            PRT_Printf(COREDUMP_PREFIX_STR "%s\n", log_buf);
            log_ptr = 0;
        }
    }
}

static int coredump_logging_backend_query(enum coredump_query_id query_id,
                      void *arg)
{
    int ret;

    switch (query_id) {
    case COREDUMP_QUERY_GET_ERROR:
        ret = error;
        break;
    default:
        ret = -ENOTSUP;
        break;
    }

    return ret;
}

static int coredump_logging_backend_cmd(enum coredump_cmd_id cmd_id,
                    void *arg)
{
    int ret;

    switch (cmd_id) {
    case COREDUMP_CMD_CLEAR_ERROR:
        ret = 0;
        error = 0;
        break;
    default:
        ret = -ENOTSUP;
        break;
    }

    return ret;
}


struct coredump_backend_api coredump_backend_print = {
    .start = coredump_logging_backend_start,
    .end = coredump_logging_backend_end,
    .buffer_output = coredump_logging_backend_buffer_output,
    .query = coredump_logging_backend_query,
    .cmd = coredump_logging_backend_cmd,
};

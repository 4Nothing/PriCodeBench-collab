#include "test_utils.h"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

static FILE *capture_file = NULL;
static int stdout_fd = -1;

void tests_init(TestStats *s)
{
    s->total = 0;
    s->passed = 0;
}

void tests_print_case(const char *case_name, const char *expected, const char *actual, bool pass, TestStats *s)
{
    s->total++;
    if (pass)
        s->passed++;
    printf("[用例] %s\n", case_name);
    printf("  期望输出: %s\n", expected);
    printf("  实际输出: %s\n", actual);
    printf("  结果    : %s\n\n", pass ? "PASS" : "FAIL");
}

void tests_print_summary(const TestStats *s)
{
    double rate = (s->total == 0) ? 0.0 : (100.0 * (double)s->passed / (double)s->total);
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, s->passed, s->total);
}

void start_capture(void)
{
    if (capture_file)
        return;
    fflush(stdout);
    stdout_fd = dup(fileno(stdout));
    capture_file = tmpfile();
    if (!capture_file)
        return;
    dup2(fileno(capture_file), fileno(stdout));
}

char *stop_capture(void)
{
    if (!capture_file)
    {
        char *empty = (char *)malloc(1);
        if (empty)
            empty[0] = '\0';
        return empty;
    }
    fflush(stdout);
    dup2(stdout_fd, fileno(stdout));
    close(stdout_fd);
    stdout_fd = -1;
    
    long len;
    fseek(capture_file, 0, SEEK_END);
    len = ftell(capture_file);
    fseek(capture_file, 0, SEEK_SET);
    char *buf = (char *)malloc((size_t)len + 1);
    if (!buf)
    {
        fclose(capture_file);
        capture_file = NULL;
        return NULL;
    }
    size_t n = fread(buf, 1, (size_t)len, capture_file);
    buf[n] = '\0';

    fclose(capture_file);
    capture_file = NULL;
    return buf;
}
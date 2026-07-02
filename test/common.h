#pragma once

#include "cmdparser.h"
#include "utest.h"

#include <string.h>

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wformat-security"
#endif

extern bool g_log_switch;
#define LOG_INFO(...)                                                                                                  \
    if (g_log_switch)                                                                                                  \
    {                                                                                                                  \
        fprintf(stdout, __VA_ARGS__);                                                                                  \
    }

#define DIM(STRING)    "\x1b[2m" STRING "\x1b[0m"
#define countof(array) (sizeof(array) / sizeof(array[0]))

#define __COMBINE__(X, Y) X##Y
#define COMBINE(X, Y)     __COMBINE__(X, Y)

#if defined(_WIN32)
static FILE *cmdp_test_open_stream(char **buf, size_t *len)
{
    FILE *stream = tmpfile();
    if (buf != NULL)
    {
        *buf = NULL;
    }
    if (len != NULL)
    {
        *len = 0;
    }
    return stream;
}

static void cmdp_test_refresh_stream(FILE *stream, char **buf, size_t *len)
{
    long end_pos = ftell(stream);
    if (end_pos < 0)
    {
        end_pos = 0;
    }
    if (fseek(stream, 0, SEEK_SET) != 0)
    {
        end_pos = 0;
    }
    free(*buf);
    *buf = (char *)malloc((size_t)end_pos + 1);
    if (*buf == NULL)
    {
        *len = 0;
        return;
    }
    *len          = fread(*buf, 1, (size_t)end_pos, stream);
    (*buf)[*len] = '\0';
    fseek(stream, 0, SEEK_END);
}
#else
#define cmdp_test_open_stream open_memstream
static void cmdp_test_refresh_stream(FILE *stream, char **buf, size_t *len)
{
    (void)stream;
    (void)buf;
    (void)len;
}
#endif

#define START_CMD()                                                                                                    \
    char *r_out      = NULL;                                                                                           \
    size_t _out_len  = 0;                                                                                              \
    FILE *out_stream = cmdp_test_open_stream(&r_out, &_out_len);                                                       \
    char *r_err      = NULL;                                                                                           \
    size_t _err_len  = 0;                                                                                              \
    FILE *err_stream = cmdp_test_open_stream(&r_err, &_err_len);                                                       \
    cmdp_ctx _ctx    = {0};                                                                                            \
    cmdp_set_default_context(&_ctx);                                                                                   \
    _ctx.out_stream = out_stream;                                                                                      \
    _ctx.err_stream = err_stream;                                                                                      \
    int r_code      = 0;

#define RUN_CMD(ref_cmdp, ...)                                                                                         \
    char *COMBINE(_arg_, __LINE__)[] = {NULL, __VA_ARGS__};                                                            \
    r_code = cmdp_run(countof(COMBINE(_arg_, __LINE__)) - 1, COMBINE(_arg_, __LINE__) + 1, ref_cmdp, &_ctx);           \
    fflush(out_stream);                                                                                                \
    fflush(err_stream);                                                                                                \
    cmdp_test_refresh_stream(out_stream, &r_out, &_out_len);                                                           \
    cmdp_test_refresh_stream(err_stream, &r_err, &_err_len);                                                           \
    LOG_INFO("%s %d\n", DIM("r_code:"), r_code);                                                                       \
    LOG_INFO("%s\n%s\n", DIM("r_out:"), r_out);                                                                        \
    LOG_INFO("%s\n%s\n", DIM("r_err:"), r_err);

#define EXPECT_CMD(code, out, err)                                                                                     \
    EXPECT_EQ(code, r_code);                                                                                           \
    EXPECT_STREQ(out, r_out);                                                                                          \
    EXPECT_STREQ(err, r_err);

#define END_CMD()                                                                                                      \
    fclose(out_stream);                                                                                                \
    fclose(err_stream);                                                                                                \
    (void)r_code;                                                                                                      \
    free(r_out);                                                                                                       \
    r_out = NULL;                                                                                                      \
    free(r_err);                                                                                                       \
    r_err = NULL;

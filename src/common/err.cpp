#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstdarg>
#include <cerrno>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include "common/err.h"

void LogErr::syserr(const char *fmt, ...) {
    va_list fmt_args;
    int err = errno;

    fprintf(stderr, "ERROR: ");

    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end (fmt_args);
    fprintf(stderr, " (%d; %s)\n", err, strerror(err));
    exit(EXIT_FAILURE);
}

void LogErr::fatal(const char *fmt, ...) {
    va_list fmt_args;

    fprintf(stderr, "ERROR: ");

    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end (fmt_args);

    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

void LogErr::logerr(const char *fmt, ...) {
    va_list fmt_args;
    int err = errno;

    fprintf(stderr, "ERROR: ");

    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end (fmt_args);
    fprintf(stderr, " (%d; %s)\n", err, strerror(err));
}

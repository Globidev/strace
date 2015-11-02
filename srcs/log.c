#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#include "error.h"
#include "log.h"
#include "peek.h"
#include "syscall.h"
#include "signals.h"
#include "errno_.h"

#define MAX_LINE_SIZE 4096
#define MIN_PADDING 40
#define PLACEHOLDER_TEXT "Unknown syscall"

static void str_write(char *str, int *written, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    if (*written < MAX_LINE_SIZE) {
        *written += vsnprintf(
            str + *written,
            MAX_LINE_SIZE - *written,
            format,
            args
        );
    }
    va_end(args);
}

static void write_string(char *out, int *written, char *str)
{
    str_write(
        out,
        written,
        "\"%s\"%s, ",
        str, // TODO: escape
        str[STRING_PEEK_MAX_SIZE - 1] && strlen(str) == STRING_PEEK_MAX_SIZE ? "..." : ""
    );
    free(str);
}

static void write_array(char *out, int *written, char **array)
{
    unsigned i;

    str_write(out, written, "[");
    if (*array) {
        write_string(out, written, *array);
        for (i = 1; array[i]; ++i) {
            str_write(out, written, ", ");
            write_string(out, written, array[i]);
        }
    }
    str_write(out, written, "]");
    free(array);
}

void output_invocation(long syscall_id, syscall_arg *args)
{
    char str[MAX_LINE_SIZE];
    const syscall_info *info;
    int written;
    unsigned i;

    info = get_syscall_info(syscall_id);
    if (info) {
        written = snprintf(str, MAX_LINE_SIZE, "%s(", info->name);
        for (i = 0; i < info->arg_count; ++i) {
            switch (info->args_type[i]) {
                case int_:
                    str_write(str, &written, "%d, ", (int)(long)args[i]);
                    break ;
                case uint_:
                    str_write(str, &written, "%u, ", (unsigned)(long)args[i]);
                    break ;
                case long_:
                    str_write(str, &written, "%ld, ", (long)args[i]);
                    break ;
                case ulong_:
                    str_write(str, &written, "%lu, ", (unsigned long)args[i]);
                    break ;
                case pointer_:
                    str_write(str, &written, "%p, ", args[i]);
                    break;
                case string_:
                    write_string(str, &written, args[i]);
                    break;
                case array_:
                    write_array(str, &written, args[i]);
                    break;
                default: break;
            }
        }
        written = written > MAX_LINE_SIZE ? MAX_LINE_SIZE : written;
        if (info->arg_count)
            written -= 2; // trick to erease the extra commas
        snprintf(str + written, MAX_LINE_SIZE - written, ")");
    }
    else
        snprintf(str, 256, PLACEHOLDER_TEXT" (%ld)", syscall_id);

    fprintf(stderr, "%-*s", MIN_PADDING - 1, str);
}

void output_return_value(long value, long syscall_id)
{
    if (value < 0) {
        if (-value < ERANGE)
            fprintf(
                stderr, " = -1 %s (%s)",
                ERRNO_NAMES[-value],
                strerror(-value)
            );
        else
            fprintf(stderr, " = -1 (%s)", strerror(-value)); /* Unknown ERRNO */
    }
    else {
        (void)syscall_id; /* TODO: Display according to info->return_type */
        fprintf(stderr, " = %ld", value);
    }
    fprintf(stderr, "\n");
}

void output_unknown_return_value()
{
    fprintf(stderr, " = ?\n");
}

#define SIGINFO_OUTPUT_FORMAT "--- %s {si_signo=%d, si_code=%d, si_pid=%d, si_uid=%d} ---\n"
void output_signal(const siginfo_t *siginfo)
{
    fprintf(
        stderr,
        SIGINFO_OUTPUT_FORMAT,
        SIGNAMES[siginfo->si_signo],
        siginfo->si_signo,
        siginfo->si_code,
        siginfo->si_pid,
        siginfo->si_uid
    );
}

void output_exit(int status, int exit_code)
{
    const char *signame;

    if (WIFEXITED(status))
        fprintf(stderr, EXIT_MESSAGE, exit_code);
    else if (WIFSIGNALED(status)) {
        signame = SIGNAMES[WTERMSIG(status)];
        if (WCOREDUMP(status))
            fprintf(stderr, KILLED_AND_DUMPED_MESSAGE, signame);
        else
            fprintf(stderr, KILLED_MESSAGE, signame);
    }
}

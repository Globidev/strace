#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include "error.h"
#include "log.h"
#include "peek.h"
#include "syscall.h"
#include "signals.h"
#include "errno_.h"

#define MIN_PADDING 40
#define PLACEHOLDER_TEXT "Unknown syscall"

void output_invocation(long syscall_id, syscall_arg *args)
{
    char str[MAX_LINE_SIZE];
    const syscall_info *info;
    int written;
    unsigned i;

    info = get_syscall_info(syscall_id);
    if (info) {
        written = snprintf(str, MAX_LINE_SIZE, "%s(", info->name);
        if (info->arg_count) {
            write_arg(args[0], info->args_type[0], str, &written);
            for (i = 1; i < info->arg_count; ++i) {
                str_printf(str, &written, ", ");
                write_arg(args[i], info->args_type[i], str, &written);
            }
        }
        written = written > MAX_LINE_SIZE ? MAX_LINE_SIZE : written;
        snprintf(str + written, MAX_LINE_SIZE - written, ")");
    }
    else
        snprintf(str, MAX_LINE_SIZE, PLACEHOLDER_TEXT" (%ld)", syscall_id);

    fprintf(stderr, "%-*s", MIN_PADDING - 1, str);
}

#define RESTART_BLOCKED_ERRNO 516
#define RESTART_BLOCKED_STR " = ? ERESTART_RESTARTBLOCK (Interrupted by signal)"
void output_return_value(long value, long syscall_id, void *arg)
{
    char str[MAX_LINE_SIZE];
    const syscall_info *info;
    int written;
    char *err;

    if (value < 0) {
        err = strerror(-value);
        if (-value < ERANGE)
            written = snprintf(
                str,
                MAX_LINE_SIZE,
                " = -1 %s (%s)",
                ERRNO_NAMES[-value],
                err
            );
        else if (-value == RESTART_BLOCKED_ERRNO) /* Special case */
            written = snprintf(str, MAX_LINE_SIZE, RESTART_BLOCKED_STR);
        else /* Unknown ERRNO */
            written = snprintf(str, MAX_LINE_SIZE, " = -1 (%s)", err);
    }
    else {
        info = get_syscall_info(syscall_id);
        if (info) {
            written = snprintf(str, MAX_LINE_SIZE, " = ");
            write_arg(arg, info->return_type, str, &written);
        }
        else
            written = snprintf(str, MAX_LINE_SIZE, " = %ld", value);
    }

    snprintf(str + written, MAX_LINE_SIZE - written, "\n");
    fprintf(stderr, str);
}

void output_unknown_return_value()
{
    fprintf(stderr, " = ?\n");
}

void output_signal(const siginfo_t *siginfo)
{
    fprintf(
        stderr,
        "--- %s {si_signo=%d, si_code=%d, si_pid=%d, si_uid=%d} ---\n",
        SIGNAMES[siginfo->si_signo],
        siginfo->si_signo,
        siginfo->si_code,
        siginfo->si_pid,
        siginfo->si_uid
    );
}

#define EXIT_MESSAGE "+++ exited with %d +++\n"
#define KILLED_MESSAGE "+++ Killed by %s +++\n"
#define KILLED_AND_DUMPED_MESSAGE "+++ Killed by %s (core dumped) +++\n"

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

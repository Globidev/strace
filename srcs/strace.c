#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "strace.h"
#include "syscall.h"
#include "path.h"
#include "log.h"
#include "error.h"

extern char **environ;

static long peek_user(pid_t pid, long offset)
{
    long data;

    data = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * offset, NULL);
    if (data == -1 && errno)
        fatal(ERR_PTRACE_PEEKUSER);

    return (data);
}

static void peek_args(pid_t pid, long *args)
{
    static long REGS[MAX_ARGS] = {
        RDI,
        RSI,
        RDX,
        RCX,
    };
    int i;

    for (i = 0; i < MAX_ARGS; ++i)
        args[i] = peek_user(pid, REGS[i]);
}

static int wait_for_syscall_trap(pid_t pid, int *status)
{
    int sig_num;

    for (;;) {
        (void)ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        if (waitpid(pid, status, 0) == -1)
            return (1);

        if (WIFSTOPPED(*status)) {
            sig_num = WSTOPSIG(*status);
            if (sig_num & SYSCALL_TRAP_MASK)
                return (0);
        }
        else if (WIFEXITED(status))
            return (1);
    }
}

static int trace_process(pid_t pid)
{
    int last_status;
    int exit_code;
    long syscall_id;
    long ret_val;
    long args[MAX_ARGS];

    /* Become a tracer */
    if (ptrace(PTRACE_SEIZE, pid, NULL, PTRACE_OPTIONS))
        fatal(ERR_PTRACE_SEIZE);
    /* Trace until exited */
    for (;;) {
        /* First trap: before effectively executing the syscall */
        if (wait_for_syscall_trap(pid, &last_status))
            break ;
        else {
            syscall_id = peek_user(pid, ORIG_RAX);
            peek_args(pid, args);
            output_invocation(syscall_id, args);
            /* Second trap: after the syscall has been executed */
            if (wait_for_syscall_trap(pid, &last_status)) {
                output_unknown_return_value(); /* Too late to peek the value */
                break ;
            }
            else {
                ret_val = peek_user(pid, RAX);
                output_return_value(ret_val, syscall_id);
            }
        }
    }

    exit_code = WEXITSTATUS(last_status);
    fprintf(stderr, EXIT_MESSAGE, exit_code);
    return (exit_code);
}

int strace(char **command)
{
    char *program;
    pid_t pid;
    int exit_code;

    exit_code = 0;
    program = resolve_program_path(command[0]);
    if (!program)
        fatal(ERR_STAT);

    pid = fork();
    if (pid == -1)
        fatal(ERR_FORK);
    else if (pid == 0) {
        kill(getpid(), SIGSTOP); /* Waiting to be traced by the parent */
        execve(program, command, environ);
        fatal(ERR_EXEC);
    }
    else
        exit_code = trace_process(pid);

    return (exit_code);
}

#include <unistd.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <sys/ptrace.h>

#include "strace.h"
#include "path.h"
#include "log.h"
#include "error.h"

extern char **environ;

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

int trace_process(pid_t pid)
{
    int last_status;
    int exit_code;

    /* Become a tracer */
    if (ptrace(PTRACE_SEIZE, pid, NULL, PTRACE_OPTIONS))
        fatal(ERR_PTRACE_SEIZE);
    /* Trace until exited */
    for (;;) {
        /* First trap: before effectively executing the syscall */
        if (wait_for_syscall_trap(pid, &last_status))
            break ;
        else
            show_syscall_invocation(pid);
        /* Second trap: after the syscall has been executed */
        if (wait_for_syscall_trap(pid, &last_status)) {
            fprintf(stderr, " = ?\n"); /* Too late to peek the return value */
            break ;
        }
        else
            show_syscall_return_value(pid);
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

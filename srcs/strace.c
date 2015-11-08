#include <unistd.h>
#include <stdio.h>

#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "strace.h"
#include "syscall.h"
#include "peek.h"
#include "path.h"
#include "log.h"
#include "tools.h"
#include "error.h"

extern char **environ;

static trap_t next_trap(pid_t pid, int *status)
{
    int sig_num;

    for (;;) {
        (void)ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        if (waitpid(pid, status, 0) == -1)
            fatal(ERR_WAITPID);

        if (WIFSTOPPED(*status)) {
            sig_num = WSTOPSIG(*status);
            if (sig_num & SYSCALL_TRAP_MASK) /* Caught a syscall */
                return (syscall_);
            else /* Caught a signal */
                return (signal_);
        }
        if (WIFEXITED(status) || WIFSIGNALED(status))
            return (exit_);
    }
}

static int signal_trap(pid_t pid, int *status)
{
    siginfo_t siginfo;

    if (ptrace(PTRACE_GETSIGINFO, pid, NULL, &siginfo))
        fatal(ERR_PTRACE_GETSIGINFO);
    /* Ignoring our own traps */
    if (!(siginfo.si_signo == SIGTRAP && siginfo.si_pid == pid))
        output_signal(&siginfo);

    if (siginfo.si_signo != SIGCONT) {
        if (ptrace(PTRACE_CONT, pid, NULL, siginfo.si_signo))
            fatal(ERR_PTRACE_CONT);
    }
    if (siginfo.si_signo == SIGSTOP) {
        /* Here we need to wait for the SIGSTOP to be delivered */
        if (waitpid(pid, status, WUNTRACED) == -1)
            fatal(ERR_WAITPID);
        /* Restarting the tracee but preventing it from executing */
        if (ptrace(PTRACE_LISTEN, pid, NULL, NULL))
            fatal(ERR_PTRACE_LISTEN);
        /* Wating for a SIGCONT */
        if (waitpid(pid, status, WCONTINUED) == -1)
            fatal(ERR_WAITPID);
    }

    return (WIFEXITED(*status) || WIFSIGNALED(*status));
}

static int syscall_trap(pid_t pid, int *status)
{
    long syscall_id, ret_val;
    syscall_arg args[MAX_ARGS];
    trap_t trap;
    const syscall_info *info;
    void *value;

    /* First trap: before effectively executing the syscall */
    syscall_id = peek_user(pid, ORIG_RAX);
    peek_args(pid, syscall_id, args);
    output_invocation(syscall_id, args);

    trap = next_trap(pid, status);
    if (trap == exit_) {
        output_unknown_return_value(); /* Too late to peek the value */
        return (1);
    }
    /* Second trap: after the syscall has been executed */
    else if (trap == syscall_) {
        ret_val = peek_user(pid, RAX);
        info = get_syscall_info(syscall_id);
        value = (info ? peek_value(pid, info->return_type, ret_val) : NULL);
        output_return_value(ret_val, syscall_id, value);
    }
    else if (trap == signal_)
        return (signal_trap(pid, status));

    return (0);
}

static int trace_process(pid_t pid)
{
    int last_status;
    int exit_code;
    trap_t trap;

    /* Become a tracer */
    if (ptrace(PTRACE_SEIZE, pid, NULL, PTRACE_OPTIONS))
        fatal(ERR_PTRACE_SEIZE);
    /* Trace until exited */
    for (;;) {
        /* Waiting for a trap */
        trap = next_trap(pid, &last_status);
        if (trap == exit_)
            break ;
        else if (trap == syscall_) {
            if (syscall_trap(pid, &last_status))
                break ;
        }
        else if (trap == signal_) {
            if (signal_trap(pid, &last_status))
                break ;
        }
    }

    exit_code = get_exit_code(last_status);
    output_exit(last_status, exit_code);
    /* If terminated by a signal, we kill ourselves with that same signal */
    if (WIFSIGNALED(last_status))
        kill(getpid(), WTERMSIG(last_status));
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
    else {
        /* Waiting for the child to stop */
        if (waitpid(pid, NULL, WUNTRACED) == -1)
            fatal(ERR_WAITPID);
        exit_code = trace_process(pid);
    }

    return (exit_code);
}

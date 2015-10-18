#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <sys/ptrace.h>
#include <sys/reg.h>

#include "error.h"

void show_syscall_invocation(pid_t pid)
{
    long syscall_id;

    syscall_id = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * ORIG_RAX, NULL);
    if (syscall_id == -1 && errno)
        fatal(ERR_PTRACE_PEEKUSER);
    fprintf(stderr, "syscall %ld", syscall_id);
}

void show_syscall_return_value(pid_t pid)
{
    long return_value;

    return_value = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * RAX, NULL);
    if (return_value == -1 && errno)
        fatal(ERR_PTRACE_PEEKUSER);
    fprintf(stderr, " = %ld\n", return_value);
}

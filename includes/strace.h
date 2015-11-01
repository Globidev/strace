#pragma once

#define PROGRAM_NAME "ft_strace"

#define PTRACE_OPTIONS PTRACE_O_TRACESYSGOOD
#define SYSCALL_TRAP_MASK 0x80

#define EXIT_MESSAGE "+++ exited with %d +++\n"

int strace(char **command);

enum e_trap
{
    syscall_,
    signal_,
    exit_,
};
typedef enum e_trap trap_t;

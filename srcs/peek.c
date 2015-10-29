#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/ptrace.h>
#include <sys/reg.h>

#include "syscall.h"
#include "peek.h"
#include "error.h"

long peek_user(pid_t pid, long offset)
{
    long data;

    data = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * offset, NULL);
    if (data == -1 && errno)
        fatal(ERR_PTRACE_PEEKUSER);

    return (data);
}

static long peek_data(pid_t pid, long offset)
{
    long data;

    data = ptrace(PTRACE_PEEKDATA, pid, offset, NULL);
    if (data == -1 && errno)
        fatal(ERR_PTRACE_PEEKDATA);

    return (data);
}

static int null_byes_in_word(long word)
{
    static long low_magic = 0x0101010101010101L;
    static long high_magic = 0x8080808080808080L;

    return (((word - low_magic) & ~word & high_magic) != 0);
}

static char *peek_string(pid_t pid, long offset)
{
    long addr;
    long data;
    char *string;
    unsigned i;
    int next;

    addr = peek_user(pid, offset);
    string = (char *)malloc(sizeof(char) * (STRING_PEEK_MAX_SIZE + 1));
    string[STRING_PEEK_MAX_SIZE] = '\0';
    i = 0;
    do {
        data = peek_data(pid, addr + i);
        memcpy(string + i, &data, sizeof(long));
        i += sizeof(long);
        next = i < STRING_PEEK_MAX_SIZE;
        next &= !null_byes_in_word(data);
    } while (next);

    return (string);
}

void peek_args(pid_t pid, long syscall_id, syscall_arg *args)
{
    static long REGS[MAX_ARGS] = {
        RDI, RSI, RDX, RCX, R8, R9
    };
    const syscall_info *info;
    unsigned i;

    info = get_syscall_info(syscall_id);
    if (info) {
        for (i = 0; i < info->arg_count; ++i) {
            switch (info->args_type[i]) {
                case int_:
                case uint_:
                case long_:
                case ulong_:
                case pointer_:
                    args[i] = (void *)peek_user(pid, REGS[i]);
                    break ;
                case string_:
                    args[i] = (void *)peek_string(pid, REGS[i]);
                    break ;
                case array_: // TODO
                    args[i] = (void *)peek_user(pid, REGS[i]);
                    break ;
                default:
                    break ;
            }

        }
    }
}

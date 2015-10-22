#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/ptrace.h>
#include <sys/reg.h>

#include "syscall.h"
#include "error.h"

long peek_user(pid_t pid, long offset)
{
    long data;

    data = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * offset, NULL);
    if (data == -1 && errno)
        fatal(ERR_PTRACE_PEEKUSER);

    return (data);
}

long peek_data(pid_t pid, long offset)
{
    long data;

    data = ptrace(PTRACE_PEEKDATA, pid, offset, NULL);
    if (data == -1 && errno)
        fatal(ERR_PTRACE_PEEKDATA);

    return (data);
}

static char *peek_string(pid_t pid, long offset)
{
    long addr;
    long data;
    char *string;
    int i;

    addr = peek_user(pid, offset);
    string = (char *)malloc(sizeof(char) * 256);
    i = 0;
    do {
        data = peek_data(pid, addr + i);
        memcpy(string + i, &data, sizeof(long));
        i += sizeof(long);
    } while (data && i < 256);

    return (string);
}

void peek_args(pid_t pid, long syscall_id, syscall_arg *args)
{
    static long REGS[MAX_ARGS] = {
        RDI, RSI, RDX, RCX,
    };
    const syscall_info *info;
    unsigned i;

    info = get_syscall_info(syscall_id);
    if (info) {
        for (i = 0; i < info->arg_count; ++i) {
            switch (info->args_type[i]) {
                case int_:
                case long_:
                case pointer_:
                    args[i] = (void *)peek_user(pid, REGS[i]);
                    break ;
                case string_:
                    args[i] = (void *)peek_string(pid, REGS[i]);
                    break ;
                default:
                    break ;
            }

        }
    }
}

#include <stdlib.h>

#include "syscall.h"

const syscall_info SYSCALLS[] = {
    { "read", 3, long_ },
    { "write", 3, long_ },
    { "open", 2, int_ },
    { "close", 1, int_ },
};

const syscall_info *get_syscall_info(long syscall_id)
{
    const syscall_info *info;

    info = NULL;
    if (syscall_id < SYSCALLS_INFO_COUNT)
        info = SYSCALLS + syscall_id;

    return (info);
}

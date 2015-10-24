#include <stdlib.h>

#include "syscall.h"

const syscall_info SYSCALLS[] = {
    { "read",  long_, 3, { int_,    pointer_, long_ } },
    { "write", long_, 3, { int_,    string_,  long_ } },
    { "open",  int_,  2, { string_, int_            } },
    { "close", int_,  1, { int_                     } },
};

const syscall_info *get_syscall_info(long syscall_id)
{
    const syscall_info *info;

    info = NULL;
    if (syscall_id < SYSCALLS_INFO_COUNT)
        info = SYSCALLS + syscall_id;

    return (info);
}

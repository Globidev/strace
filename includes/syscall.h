#pragma once

enum e_return_type
{
    int_,
    long_,
    pointer_,
};
typedef enum e_return_type return_type;

struct s_syscall_info
{
    const char *name;
    unsigned arg_count;
    return_type return_type;
};
typedef struct s_syscall_info syscall_info;

#define SYSCALLS_INFO_COUNT 4
const syscall_info SYSCALLS[SYSCALLS_INFO_COUNT];

const syscall_info *get_syscall_info(long syscall_id);

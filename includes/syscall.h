#pragma once

enum e_type
{
    int_,
    long_,
    pointer_,
    string_,
    array_,
};
typedef enum e_type type;

#define MAX_ARGS 4
struct s_syscall_info
{
    const char *name;
    type return_type;
    unsigned arg_count;
    type args_type[MAX_ARGS];
};
typedef struct s_syscall_info syscall_info;
typedef void * syscall_arg;

#define SYSCALLS_INFO_COUNT 4
const syscall_info SYSCALLS[SYSCALLS_INFO_COUNT];

const syscall_info *get_syscall_info(long syscall_id);
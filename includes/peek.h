#pragma once

#include <stdlib.h>

#include "syscall.h"

#define STRING_PEEK_MAX_SIZE 32

long peek_user(pid_t pid, long offset);
void peek_args(pid_t pid, long syscall_id, syscall_arg *args);

#pragma once

#include "syscall.h"

long peek_user(pid_t pid, long offset);
void peek_args(pid_t pid, long syscall_id, syscall_arg *args);

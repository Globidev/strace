#pragma once

#include "syscall.h"

void output_invocation(long syscall_id, syscall_arg *args);
void output_return_value(long value, long syscall_id);
void output_unknown_return_value();

void output_signal(const siginfo_t *siginfo);

#define EXIT_MESSAGE "+++ exited with %d +++\n"
#define KILLED_MESSAGE "+++ Killed by %s +++\n"
#define KILLED_AND_DUMPED_MESSAGE "+++ Killed by %s (core dumped) +++\n"
void output_exit(int status, int exit_code);

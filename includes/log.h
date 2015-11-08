#pragma once

#include <signal.h>

#include "syscall.h"

#define MAX_LINE_SIZE 4096

void str_printf(char *str, int *written, const char *format, ...);
void write_arg(void *arg, type arg_type, char *str, int *written);

void output_invocation(long syscall_id, syscall_arg *args);
void output_return_value(long value, long syscall_id, void *args);
void output_unknown_return_value();

void output_signal(const siginfo_t *siginfo);

void output_exit(int status, int exit_code);

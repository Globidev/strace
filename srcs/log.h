#pragma once

#include "syscall.h"

void output_invocation(long syscall_id, syscall_arg *args);
void output_return_value(long value, long syscall_id);
void output_unknown_return_value();

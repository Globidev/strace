#pragma once

void output_invocation(long syscall_id, long *args);
void output_return_value(long value, long syscall_id);
void output_unknown_return_value();

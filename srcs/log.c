#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "error.h"
#include "syscall.h"

void output_invocation(long syscall_id, long *args)
{
    char invocation_str[256];
    const syscall_info *info;
    int written;
    unsigned i;

    info = get_syscall_info(syscall_id);
    if (info) {
        written = 0;
        written = snprintf(invocation_str, 256, "%s(", info->name);
        for (i = 0; i < info->arg_count; ++i) {
            switch (info->args_type[i]) {
                case int_:     written += snprintf(invocation_str + written, 256 - written, "%d, ", (int)args[i]); break;
                case long_:    written += snprintf(invocation_str + written, 256 - written, "%ld, ", args[i]); break;
                case pointer_: written += snprintf(invocation_str + written, 256 - written, "%p, ", (void *)args[i]); break;
                case string_:  written += snprintf(invocation_str + written, 256 - written, "%p, ", (void *)args[i]); break; // TODO
                case array_:   written += snprintf(invocation_str + written, 256 - written, "[...]"); break; // TODO
                default: break;
            }
        }
        snprintf(invocation_str + written - 2, 256 - written, ")");
    }
    else
        snprintf(invocation_str, 256, "unknown syscall (%ld)", syscall_id);

    fprintf(stderr, "%-39s", invocation_str);
}

void output_return_value(long value, long syscall_id)
{
    if (value < 0)
        fprintf(stderr, " = -1 (%s)", strerror(-value)); /* TODO: show ERRNO */
    else {
        (void)syscall_id; /* TODO: Display according to info->return_type */
        fprintf(stderr, " = %ld", value);
    }
    fprintf(stderr, "\n");
}

void output_unknown_return_value()
{
    fprintf(stderr, " = ?\n");
}

#define _GNU_SOURCE /* Enabling asprintf */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "error.h"
#include "syscall.h"

void output_invocation(long syscall_id)
{
    char *invocation_str;
    const syscall_info *info;

    info = get_syscall_info(syscall_id);
    if (info)
        asprintf(&invocation_str, "%s()", info->name);
    else
        asprintf(&invocation_str, "unknown syscall (%ld)", syscall_id);

    fprintf(stderr, "%-39s", invocation_str);
    free(invocation_str);
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

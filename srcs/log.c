#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "error.h"
#include "peek.h"
#include "syscall.h"

#define MAX_LINE_SIZE 256
#define MIN_PADDING 40
#define PLACEHOLDER_TEXT "Unknown syscall"

static void str_write(char *str, int *written, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    if (*written < MAX_LINE_SIZE) {
        *written += vsnprintf(
            str + *written,
            MAX_LINE_SIZE - *written,
            format,
            args
        );
    }
    va_end(args);
}

void output_invocation(long syscall_id, syscall_arg *args)
{
    char str[MAX_LINE_SIZE];
    const syscall_info *info;
    int written;
    unsigned i;

    info = get_syscall_info(syscall_id);
    if (info) {
        written = snprintf(str, MAX_LINE_SIZE, "%s(", info->name);
        for (i = 0; i < info->arg_count; ++i) {
            switch (info->args_type[i]) {
                case int_:
                    str_write(str, &written, "%d, ", (int)(long)args[i]);
                    break ;
                case uint_:
                    str_write(str, &written, "%u, ", (unsigned)(long)args[i]);
                    break ;
                case long_:
                    str_write(str, &written, "%ld, ", (long)args[i]);
                    break ;
                case ulong_:
                    str_write(str, &written, "%lu, ", (unsigned long)args[i]);
                    break ;
                case pointer_:
                    str_write(str, &written, "%p, ", args[i]);
                    break;
                case string_:
                    str_write(
                        str,
                        &written,
                        "\"%s\"%s, ",
                        (char *)args[i], // TODO: escape
                        ((char *)args[i])[STRING_PEEK_MAX_SIZE - 1] ? "..." : ""
                    );
                    break;
                case array_:
                    str_write(str, &written, "[...], "); // TODO
                    break;
                default: break;
            }
        }
        if (info->arg_count)
            written -= 2; // trick to erease the extra commas
        snprintf(str + written, MAX_LINE_SIZE - written, ")");
    }
    else
        snprintf(str, 256, PLACEHOLDER_TEXT" (%ld)", syscall_id);

    fprintf(stderr, "%-*s", MIN_PADDING - 1, str);
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

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/wait.h>

int null_byes_in_word(long word)
{
    static long low_magic = 0x0101010101010101L;
    static long high_magic = 0x8080808080808080L;

    return (((word - low_magic) & ~word & high_magic) != 0);
}

int get_exit_code(int status)
{
    if (WIFEXITED(status))
        return (WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        return (CHAR_MAX + 1 + WTERMSIG(status));
    /* Should never reach here (otherwise the process is not exited) */
    return (EXIT_FAILURE);
}

char *escape(const char *str)
{
    static const char escape_chars[] = {
        '\0',  '\a',  '\b',  '\t',  '\n',  '\v',  '\f',  '\r',  '\\'
    };
    static const char* escape_strs[] = {
        "\\0", "\\a", "\\b", "\\t", "\\n", "\\v", "\\f", "\\r", "\\\\"
    };
    char *escaped;
    size_t size;
    int written;
    unsigned i, found;

    /* Maximum 4 escaping chars per char present */
    size = strlen(str) * 4;
    escaped = (char *)malloc(sizeof(char) * (size + 1));
    written = 0;
    for (; *str; ++str) {
        if (isprint(*str))
            written += sprintf(escaped + written, "%c", *str);
        else {
            found = 0;
            for (i = 0; i < sizeof(escape_chars) / sizeof(char); ++i) {
                if (*str == escape_chars[i]) {
                    written += sprintf(escaped + written, "%s", escape_strs[i]);
                    found = 1;
                    break ;
                }
            }
            if (!found)
                written += sprintf(escaped + written, "\\%o", *str);
        }
    }
    escaped[written] = '\0';

    return (escaped);
}

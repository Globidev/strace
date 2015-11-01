#include <stdlib.h>
#include <limits.h>

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

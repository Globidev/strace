#include <stdlib.h>

#include "strace.h"

int main(int argc, char **argv)
{
    int exit_code;

    if (argc >= 2) {
        exit_code = strace(argv + 1);
        return (exit_code);
    }
    return (EXIT_FAILURE);
}

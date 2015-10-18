#include <stdlib.h>
#include <stdio.h>

#include "strace.h"

static void show_usage()
{
    printf("usage: %s PROG [ARGS]\n", PROGRAM_NAME);
}

int main(int argc, char **argv)
{
    int exit_code;

    if (argc < 2) {
        show_usage();
        return (EXIT_FAILURE);
    }
    else {
        exit_code = strace(argv + 1);
        return (exit_code);
    }
}

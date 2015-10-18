#include <stdio.h>
#include <stdlib.h>

#include "strace.h"

void fatal(const char *prefix)
{
    perror(prefix);
    exit(EXIT_FAILURE);
}

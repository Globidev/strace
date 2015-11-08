#define _GNU_SOURCE /* Enabling asprintf */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "path.h"

static int should_skip_path_search(const char *program)
{
    int is_relative;
    int is_absolute;

    is_relative = (program[0] == '.');
    is_absolute = (program[0] == '/');

    return (is_relative || is_absolute);
}

static char *find_in_path(const char *program, char *path)
{
    char *prefix;
    char *concatenated, *normalized;

    prefix = strtok(path, ENV_PATH_SEPARATOR);
    while (prefix) {
        asprintf(&concatenated, "%s/%s", prefix, program);
        normalized = realpath(concatenated, NULL); /* Normalizing */
        if (!normalized)
            normalized = strdup(concatenated); /* Fallback for access */
        free(concatenated);
        if (access(normalized, F_OK) == 0)
            return (normalized);
        else
            free(normalized);
        prefix = strtok(NULL, ENV_PATH_SEPARATOR);
    }

    return (NULL);
}

char *resolve_program_path(const char *program)
{
    char *env_path;
    char *path;

    if (should_skip_path_search(program)) {
        if (access(program, F_OK) == 0)
            path = strdup(program);
        else
            path = NULL;
    }
    else {
        env_path = getenv(ENV_PATH);
        if (!env_path) /* No path variable */
            path = strdup(program);
        else
            path = find_in_path(program, env_path);
    }

    return (path);
}

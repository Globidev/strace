#pragma once

#define ERR_FORK "Fork failed"
#define ERR_EXEC "Exec failed"
#define ERR_STAT "Stat failed"

void fatal(const char *error);

#pragma once

#define ERR_FORK "fork failed"
#define ERR_EXEC "exec failed"
#define ERR_STAT "stat failed"
#define ERR_WAITPID "waitpid failed"

#define ERR_PTRACE_SEIZE "PTRACE_SEIZE failed"
#define ERR_PTRACE_PEEKUSER "PTRACE_PEEKUSER failed"
#define ERR_PTRACE_PEEKDATA "PTRACE_PEEKDATA failed"
#define ERR_PTRACE_GETSIGINFO "PTRACE_GETSIGINFO failed"
#define ERR_PTRACE_CONT "PTRACE_CONT failed"
#define ERR_PTRACE_LISTEN "PTRACE_LISTEN failed"

void fatal(const char *error);

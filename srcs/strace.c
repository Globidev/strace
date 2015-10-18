#include <unistd.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <sys/ptrace.h>
#include <sys/reg.h>

#include "error.h"

extern char **environ;

int strace(char **command)
{
    pid_t pid;

    pid = fork();
    if (pid == -1)
        fatal("Fork failed");
    else if (pid == 0) {
        kill(getpid(), SIGSTOP);
        execve(command[0], command, environ);
        fatal("exec failed");
    }
    else {
        ptrace(PTRACE_SEIZE, pid, 0, PTRACE_O_TRACESYSGOOD);
        int i = 0;
        for(;;) {
            
            for (;;) {
                ptrace(PTRACE_SYSCALL, pid, 0, 0);
                int status;
                waitpid(pid, &status, 0);
                if (WIFSTOPPED(status) && WSTOPSIG(status) & 0x80)
                    break ;
                else if (WIFEXITED(status))
                    return(0);
            }

            int syscall = ptrace(
                PTRACE_PEEKUSER,
                pid,
                sizeof(long)* (i++ % 2 ? RAX : ORIG_RAX)
            );
            printf(i % 2 ? "syscall(%d) = " : "%x\n", syscall);
        }
    }
    return (0);
}

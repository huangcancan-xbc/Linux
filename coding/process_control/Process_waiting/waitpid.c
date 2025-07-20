#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{
    pid_t pid = fork();

    if (pid == 0)
    {
        exit(10);
    }
    else
    {
        int status;
        pid_t wpid;
        while ((wpid = waitpid(pid, &status, WNOHANG)) == 0)
        {
            printf("父进程忙别的事...\n");
            sleep(1);
        }
        if (WIFEXITED(status))
        {
            printf("子进程退出码 = %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}

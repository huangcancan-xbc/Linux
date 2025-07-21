#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char* const argv[] = { "ps", "-ef", NULL };
    char* const envp[] = { "PATH=/bin:/usr/bin", "TERM=console", NULL };

    // execl: 需要完整路径
    if (execl("/bin/ps", "ps", "-ef", NULL) == -1)
    {
        perror("execl");
        exit(1);
    }

    // execlp: 使用 PATH 环境变量
    if (execlp("ps", "ps", "-ef", NULL) == -1)
    {
        perror("execlp");
        exit(1);
    }

    // execle: 需要自己设置环境变量
    if (execle("ps", "ps", "-ef", NULL, envp) == -1)
    {
        perror("execle");
        exit(1);
    }

    // execv: 参数通过数组传递
    if (execv("/bin/ps", argv) == -1)
    {
        perror("execv");
        exit(1);
    }

    // execvp: 使用 PATH 环境变量
    if (execvp("ps", argv) == -1)
    {
        perror("execvp");
        exit(1);
    }

    // execve: 需要自己设置环境变量
    if (execve("/bin/ps", argv, envp) == -1)
    {
        perror("execve");
        exit(1);
    }

    return 0;
}

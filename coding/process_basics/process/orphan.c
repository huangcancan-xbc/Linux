#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    pid_t pid = fork();                 // 创建子进程

    if (pid < 0)
    {
        perror("fork 失败");
        exit(1);
    }
    else if (pid == 0)                  // 子进程逻辑
    {
        sleep(1);                       // 确保父进程先退出
        printf("子进程 PID: %d, PPID: %d\n", getpid(), getppid());
        while (1)
        {
            sleep(10);                  // 保持子进程存活，方便观察
        }
    }
    else                                // 父进程逻辑
    {
        printf("父进程 PID: %d 创建了子进程 PID: %d，然后退出\n", getpid(), pid);
        exit(0);                        // 父进程主动退出，产生孤儿进程
    }

    return 0;
}

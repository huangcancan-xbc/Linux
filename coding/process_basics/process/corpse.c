#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    pid_t pid = fork();                 // 创建子进程

    if (pid < 0)
    {
        perror("fork失败");
        exit(1);
    }
    else if (pid == 0)                  // 子进程
    {
        printf("子进程（PID：%d）正在运行...\n", getpid());
        sleep(5);                       // 子进程运行5秒后退出
        printf("子进程（PID：%d）即将退出...\n", getpid());
        exit(0);
    }
    else                                // 父进程
    {
        printf("父进程（PID：%d）正在运行...\n", getpid());
        sleep(30);                      // 父进程等待30秒（不调用wait()，导致子进程成为僵尸进程）
        printf("父进程结束。\n");
    }

    return 0;
}

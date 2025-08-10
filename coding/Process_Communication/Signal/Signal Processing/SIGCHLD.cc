#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
using namespace std;

void handler(int signo)
{
    // 回收所有退出的子进程，防止僵尸进程
    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
        printf("子进程已退出，回收完成\n");
    }
}

int main()
{
    signal(SIGCHLD, handler);

    pid_t pid = fork();
    if (pid == 0)
    {
        printf("子进程运行中，PID = %d\n", getpid());
        sleep(2);
        exit(0);                // 正常退出，触发 SIGCHLD
    }
    else
    {
        while (1)
        {
            printf("父进程在运行中，PID = %d\n", getpid());
            sleep(1);
        }
    }

    return 0;
}
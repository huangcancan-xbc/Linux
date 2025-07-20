#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
int main()
{
    pid_t id = fork();          // 创建子进程

    if (id == 0)
    {
        int time = 5;
        int n = 0;
        while (n < time)
        {
            printf("我是子进程，我已经运行了:%d秒 PID:%d   PPID:%d\n", n + 1, getpid(), getppid());
            sleep(1);
            n++;
        }

        exit(244);              // 子进程退出
    }

    int status = 0;             // 状态
    pid_t ret = waitpid(id, &status, 0);        // 参数3 为0，为默认选项

    if (ret == -1)
    {
        printf("进程等待失败！进程不存在！\n");
    }
    else if (ret == 0)
    {
        printf("子进程还在运行中！\n");
    }
    else
    {
        printf("进程等待成功，子进程已被回收\n");
    }

    printf("我是父进程, PID:%d   PPID:%d\n", getpid(), getppid());

    //通过 status 判断子进程运行情况
    if ((status & 0x7F))
    {
        printf("子进程异常退出，core dump：%d   退出信号：%d\n", (status >> 7) & 1, (status & 0x7F));
    }
    else
    {
        printf("子进程正常退出，退出码：%d\n", (status >> 8) & 0xFF);
    }

    return 0;
}

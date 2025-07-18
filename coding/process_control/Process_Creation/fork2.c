#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>  // 定义 pid_t 类型
#include <unistd.h>     // 定义 fork() 函数
#define N 5
void runChild()
{
    int cnt = 10;
    while (cnt)
    {
        printf("我是子进程：%d，ppid:%d\n", getpid(), getppid());
        sleep(1);
        cnt--;
    }
}

int main()
{
    int i = 0;					    // 避免 C99 不支持报错
    for (i = 0; i < N; i++)
    {
        pid_t pid = fork();
        if (pid == 0)               // 子进程
        {
            runChild();
            exit(0);                // 子进程退出，避免继续循环
        }
        else if (pid < 0)           // fork失败
        {
            perror("fork");
            exit(1);
        }
        // 父进程继续循环
    }

    return 0;
}

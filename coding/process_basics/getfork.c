#include <unistd.h>
#include <stdio.h>

int main()
{
    pid_t pid = fork();

    if (pid < 0)
    {
        fprintf(stderr, "创建子进程失败");                             // 将标准错误输出的信息改为中文
        return 1;
    }
    else if (pid == 0)
    {
        printf("子进程PID=%d, 父进程PPID=%d\n", getpid(), getppid());  // 子进程中打印的信息改为中文
    }
    else
    {
        printf("父进程PID=%d, 创建的子进程PID=%d\n", getpid(), pid);   // 父进程中打印的信息改为中文
    }

    return 0;
}

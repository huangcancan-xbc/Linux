#include <stdio.h>
#include <unistd.h>  // 包含 fork() 的头文件
#include <stdlib.h>  // 包含 exit() 的头文件

int main()
{
    pid_t pid = fork();         // 创建子进程

    if (pid == 0)               // 子进程：使用 execl 替换为新程序
    {
        printf("我是子进程，PID: %d，现在执行 execl 替换为 ls 命令\n", getpid());
        execl("/bin/ls", "ls", "-l", NULL);

        perror("execl failed"); // 若 execl 出错，才会继续执行下面语句
        exit(1);
    }
    else if (pid > 0)           // 父进程继续执行原有逻辑
    {
        printf("我是父进程，PID: %d，创建了子进程: %d\n", getpid(), pid);
        sleep(2);
        printf("父进程结束\n");
    }
    else
    {
        perror("fork failed");
        return 1;
    }

    return 0;
}

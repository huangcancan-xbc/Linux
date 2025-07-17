#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int g_val = 100;                // 全局变量，存储在数据段

int main()
{
    pid_t id = fork();          // 调用 fork() 创建子进程
    if (id == 0)                // 子进程
    {              
        g_val = 200;            // 修改全局变量的值
        
        printf("child:PID:%d, PPID:%d, g_val:%d, &g_val:%p\n", getpid(), getppid(), g_val, &g_val);
    }
    else if (id > 0)            // 父进程
    {  
        sleep(3);               // 父进程睡眠 3 秒，等待子进程运行
        
        printf("father:PID:%d, PPID:%d, g_val:%d, &g_val:%p\n", getpid(), getppid(), g_val, &g_val);
    }
    else                        // fork() 失败
    {  
        // 处理 fork() 错误
    }

    return 0;
}

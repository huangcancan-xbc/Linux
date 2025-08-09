#include <stdio.h>
#include <unistd.h>
#include <signal.h>
using namespace std;

void printPending(sigset_t *pending)
{
    int i = 1;
    for (i = 31; i >= 1; i--)
    {
        if (sigismember(pending, i))            // 检查信号 i 是否在 pending 集合中
        {
            printf("1");                        // 在 pending 集合中
        }
        else
        {
            printf("0");                        // 不在 pending 集合中
        }
    }

    printf("\n");
}

int main()
{
    sigset_t set, oset;
    sigemptyset(&set);
    sigemptyset(&oset);

    sigaddset(&set, 2);                         // 向 set 中添加 2 号信号（SIGINT，即 Ctrl+C）
    sigprocmask(SIG_SETMASK, &set, &oset);

    
    sigset_t pending;                           // 定义 pending 信号集，用于后续获取当前 pending 的信号
    sigemptyset(&pending);

    while (1)                                   // 无限循环，持续检测 pending 信号集
    {
        sigpending(&pending);                   // 获取当前 pending 信号集
        printPending(&pending);                 // 打印 pending 信号位图（1 表示 pending，0 表示未 pending）
        sleep(1);                               // 每隔 1 秒检测一次
    }

    return 0;
}
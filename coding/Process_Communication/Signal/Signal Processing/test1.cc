#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int signo)
{
    printf("进入 handler\n");
    sleep(3);                                   // 模拟处理过程
    printf("退出 handler\n");
}

int main()
{
    struct sigaction act;                       // 定义信号处理结构体
    act.sa_handler = handler;                   // 设置信号处理函数
    sigemptyset(&act.sa_mask);                  // 阻塞信号集为空
    act.sa_flags = 0;                           // 信号处理标志位

    sigaction(SIGINT, &act, NULL);              // 注册信号处理函数

    sigset_t set;                               // 定义信号集
    sigemptyset(&set);                          // 信号集为空
    sigaddset(&set, SIGINT);                    // 加入 SIGINT 信号
    sigprocmask(SIG_BLOCK, &set, NULL);         // 阻塞 SIGINT 信号

    printf("请在 5 秒内按 Ctrl+C（SIGINT 会被 pending）...\n");
    sleep(5);                                   // 等待 5 秒

    sigset_t pending;                           // 定义 pending 信号集  
    sigpending(&pending);                       // 获取 pending 信号集
    if (sigismember(&pending, SIGINT))
    {
        printf("SIGINT 当前处于 pending 状态（位为 1）\n");
    }

    sigprocmask(SIG_UNBLOCK, &set, NULL);       // 解除 SIGINT 信号的阻塞
    pause();                                    // 等待信号处理完成

    sigpending(&pending);                       // 获取 pending 信号集
    if (!sigismember(&pending, SIGINT))
    {
        printf("SIGINT 的 pending 位已被清 0\n");
    }

    return 0;
}
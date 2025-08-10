#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int signo)
{
    printf("进入 handler，此时 SIGINT 被屏蔽\n");

    sigset_t blocked;                                    // 保存当前信号集
    sigprocmask(0, NULL, &blocked);                      // 获取当前信号集

    if (sigismember(&blocked, SIGINT))                   // 判断 SIGINT 是否被屏蔽
    {
        printf("SIGINT 当前被阻塞（屏蔽中）\n");
    }
    else
    {
        printf("SIGINT 未被阻塞\n");
    }

    sleep(5); // 模拟处理过程
    printf("退出 handler\n");
}

int main()
{
    struct sigaction act;                                // 信号处理结构体
    act.sa_handler = handler;                            // 设置信号处理函数
    sigemptyset(&act.sa_mask);                           // 初始化信号集
    act.sa_flags = 0;                                    // 信号处理标志

    sigaction(SIGINT, &act, NULL);                       // 注册信号处理函数

    printf("发送 SIGINT 将触发 handler\n");
    while (1)
    {
        pause();                                         // 阻塞进程，等待信号
    }

    return 0;
}
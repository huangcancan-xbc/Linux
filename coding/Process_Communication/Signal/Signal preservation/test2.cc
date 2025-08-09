#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
using namespace std;

// 打印 pending 位图（31 位），1 表示 pending，0 表示未 pending
void printPending(sigset_t *pending)
{
    for (int i = 31; i >= 1; i--)
    {
        if (sigismember(pending, i))
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
    }
    
    printf("\n");
}

int main()
{
    sigset_t block_set, old_mask;
    sigemptyset(&block_set);
    sigemptyset(&old_mask);

    // 阻塞 1~31 号信号
    for (int i = 1; i <= 31; i++)
    {
        sigaddset(&block_set, i);
    }

    // 设置阻塞信号集（屏蔽所有 1~31 号信号）
    sigprocmask(SIG_SETMASK, &block_set, &old_mask);

    printf("已屏蔽 1~31 号信号，现在你可以发送信号了。\n");
    printf("例如：kill -1 PID、kill -2 PID ... kill -31 PID\n");

    sigset_t pending;

    while (1)
    {
        sigemptyset(&pending);
        sigpending(&pending); // 获取当前 pending 信号集合
        printPending(&pending);
        sleep(1);
    }

    return 0;
}
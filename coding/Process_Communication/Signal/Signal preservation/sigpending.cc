#include <iostream>
#include <csignal>
#include <unistd.h>
using namespace std;

int main()
{
    sigset_t block_set, pending_set;                    // 定义两个信号集合
    sigemptyset(&block_set);                            // 初始化 block_set 为一个空集合（所有信号都不阻塞）
    sigaddset(&block_set, SIGINT);                      // 向 block_set 中添加 SIGINT 信号（Ctrl+C），表示我们想阻塞这个信号

    // 使用 sigprocmask 函数设置当前进程的信号屏蔽字（block 集合），SIG_BLOCK 表示将 block_set 中的信号添加到当前阻塞集合中（叠加），第三个参数为 nullptr，表示不保存原来的阻塞集合
    sigprocmask(SIG_BLOCK, &block_set, nullptr);

    cout << "请在 5 秒内按 Ctrl+C（不会立刻触发 handler）..." << endl;      // 在 5 秒内按下 Ctrl+C，此时 SIGINT 会被阻塞，进入 pending 状态
    sleep(5);                                           // 程序暂停 5 秒，等待按下 Ctrl+C

    sigpending(&pending_set);                           // 获取当前进程的“未决信号集合”，pending_set 保存当前所有“已产生但未处理”的信号

    if (sigismember(&pending_set, SIGINT))              // 检查 SIGINT 是否在 pending_set 中
    {
        cout << "SIGINT 当前处于 pending 状态。" << endl;// 如果 SIGINT 在 pending 集合中，说明它已经产生但被阻塞了
    } 
    else 
    {
        cout << "SIGINT 没有 pending。" << endl;        // 如果 SIGINT 不在 pending 集合中，说明它没被发送，或已经被处理
    }

    sigprocmask(SIG_UNBLOCK, &block_set, nullptr);      // 解除对 SIGINT 的阻塞，让它可以正常递达
    sleep(2);                                           // 再等 2 秒，如果之前按过 Ctrl+C，此时 SIGINT 会递达并触发默认处理（终止程序）

    return 0;
}
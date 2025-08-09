#include <iostream>
#include <csignal>
#include <unistd.h>
using namespace std;

void myhandler(int signo)
{
    cout << "收到信号：" << signo << endl;
}

int main()
{
    signal(SIGINT, myhandler);              // 设置 SIGINT 信号的处理函数为 myhandler，SIGINT 是 Ctrl+C 发出的中断信号

    sigset_t mask, oldmask;                 // 定义两个信号集：mask：新的信号阻塞集，oldmask：保存原来的信号阻塞集，用于之后恢复
    sigemptyset(&mask);                     // 初始化 mask 为一个空集合（所有信号都未被阻塞）
    sigaddset(&mask, SIGINT);               // 向 mask 中添加 SIGINT 信号，表示我们想阻塞 SIGINT（Ctrl+C）
    sigprocmask(SIG_BLOCK, &mask, &oldmask);// 使用 sigprocmask 设置当前进程的阻塞信号集：SIG_BLOCK（添加），mask（新的阻塞集），oldmask（保存原来的阻塞集）

    cout << "SIGINT 被阻塞，按 Ctrl+C 不会触发 handler..." << endl;     // 此时 SIGINT 被阻塞，即使按下 Ctrl+C 也不会立即触发信号处理函数
    sleep(5);                               // 程序休眠 5 秒钟，期间 SIGINT 被阻塞

    sigprocmask(SIG_SETMASK, &oldmask, nullptr);        // 恢复原来的信号阻塞集（解除 SIGINT 的阻塞），SIG_SETMASK：将当前阻塞集完全替换为 mask 中的集合，oldmask：之前保存的阻塞集合

    cout << "解除阻塞，SIGINT 可再次递达..." << endl;     // 现在 SIGINT 可以正常递达，再次按下 Ctrl+C 就会触发 myhandler
    sleep(5);                               // 再次休眠 5 秒，此时可以接收到 SIGINT 信号

    return 0;
}
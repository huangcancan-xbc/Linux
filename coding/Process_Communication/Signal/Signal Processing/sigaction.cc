#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <cstring>
using namespace std;

void handler(int signum)
{
    cout << "收到信号: " << signum << endl;
}

int main()
{
    struct sigaction act;                       // 用于设置新行为
    memset(&act, 0, sizeof(act));               // 初始化为 0，避免随机值干扰
    act.sa_handler = handler;                   // 设置信号处理函数
    sigemptyset(&act.sa_mask);                  // 清空屏蔽字
    act.sa_flags = 0;                           // 无特殊行为

    if (sigaction(SIGINT, &act, nullptr) == -1) // 设置 SIGINT（如按 Ctrl+C） 的处理方式
    {
        perror("sigaction error");
        return 1;
    }

    while (true)                                // 模拟服务一直运行
    {
        cout << "程序运行中...（按 Ctrl+C 测试信号）" << endl;
        sleep(2);
    }

    return 0;
}
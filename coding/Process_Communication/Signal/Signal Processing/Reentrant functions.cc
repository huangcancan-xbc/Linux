#include <iostream>
#include <csignal>
#include <unistd.h>
#include <cstring>
using namespace std;

// 信号处理函数，使用不可重入函数 printf（不推荐）
void unsafe_handler(int signo)
{
    printf("非可重入函数 printf 在信号处理函数中执行了！\n");   // 不推荐
}

// 信号处理函数，使用可重入函数 write（推荐）
void safe_handler(int signo)
{
    const char* msg = "可重入函数 write 在信号处理函数中执行了！\n";
    write(STDOUT_FILENO, msg, strlen(msg));                  // 推荐使用 write
}

int main()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));

    // 分别运行 unsafe_handler 和 safe_handler并观察结果
    // act.sa_handler = unsafe_handler;                   // 非安全版本
    act.sa_handler = safe_handler;                          // 安全版本

    sigaction(SIGINT, &act, nullptr);

    // 主循环
    while (true)
    {
        cout << "主程序运行中，按 Ctrl+C 触发信号..." << endl;
        sleep(1);
    }

    return 0;
}

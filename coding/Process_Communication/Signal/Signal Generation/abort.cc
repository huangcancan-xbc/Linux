#include <iostream>
#include <csignal>
#include <cstdlib>
using namespace std;

void myhandler(int sig)
{
    cout << "收到 SIGABRT: " << sig << endl;
    exit(0);
}

int main()
{
    signal(SIGABRT, myhandler);  // 注册自定义处理

    cout << "程序即将收到 abort 信号立刻异常终止..." << endl;
    abort();                     // 固定发 6号信号 SIGABRT

    return 0;
}
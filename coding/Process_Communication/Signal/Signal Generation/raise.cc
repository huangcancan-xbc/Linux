#include <iostream>
#include <csignal>
using namespace std;

void myhandler(int sig)
{
    cout << "收到信号: " << sig << endl;
}

int main()
{
    signal(2, myhandler);  // 注册自定义处理

    cout << "程序准备即将被终止！" << endl;
    raise(2);              // 自己给自己发 SIGTERM

    return 0;
}
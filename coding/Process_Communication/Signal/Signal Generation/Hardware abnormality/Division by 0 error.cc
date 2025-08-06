#include <iostream>
#include <signal.h>
#include <unistd.h>
using namespace std;

void myhandler(int sig)
{
    cout << "收到的信号是：" << sig << endl;
    exit(0);                   // 注释掉会导致程序无限循环，可以尝试注释掉观察现象
}

int main()
{
    signal(SIGFPE, myhandler);
    
    cout << "这是除以 0 之前！" << endl;
    sleep(1);

    int a = 10;
    a /= 0;                    // 触发 SIGFPE 信号

    cout << "这是除以 0 之后！" << endl;
    sleep(1);

    return 0;
}
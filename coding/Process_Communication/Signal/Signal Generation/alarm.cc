#include <iostream>
#include <unistd.h>   // alarm
#include <signal.h>   // signal
using namespace std;

void myhandler(int sig)
{
    cout << "收到 SIGALRM 信号，编号：" << sig << endl;
    exit(0);          // 如果注释掉这里，也必须要注释掉 signal 函数（或者单独注释掉 alarm 函数），运行才会只显示前5秒的输出
}

int main()
{
    signal(SIGALRM, myhandler);

    cout << "设置闹钟，5 秒后触发 SIGALRM..." << endl;

    alarm(5);          // 5 秒后产生 SIGALRM

    for (int i = 1; i <= 10; ++i)
    {
        cout << "程序运行中: " << i << " 秒" << endl;   // 程序只会运行到 i=5 秒，然后被 SIGALRM 信号中断
        sleep(1);
    }

    cout << "程序结束。" << endl;

    return 0;
}
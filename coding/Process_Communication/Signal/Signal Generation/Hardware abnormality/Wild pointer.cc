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
    signal(SIGSEGV, myhandler);
    
    cout << "这是程序开始之前！" << endl;
    sleep(1);

    int *p = nullptr;
    *p = 10;                    // 野指针，会触发信号 SIGSEGV

    cout << "这是程序结束之后！" << endl;
    sleep(1);

    return 0;
}
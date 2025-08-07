#include <iostream>
#include <signal.h>
#include <unistd.h>
using namespace std;

void myhandler(int signum)
{
    cout << "收到信号编号: " << signum << endl;
    // 当 signal 使用了自定义“捕获”函数，使用 CTRL+C 退出程序时会无法正常退出，此时在此处调用 exit 函数即可正常退出程序。
    // 不使用 exit 函数，想要退出可以使用 CTRL+\ 组合键。
    exit(0);
}

int main()
{
    signal(2, myhandler);
    // 也可以写成 signal(SIGINT, myhandler);
    cout << "运行中，按 Ctrl+C..." << endl;

    while (true)
    {
        sleep(1);
    }

    return 0;
}
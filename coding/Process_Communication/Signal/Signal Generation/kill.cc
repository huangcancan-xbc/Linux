#include <iostream>
#include <unistd.h>                     // fork(), sleep()
#include <signal.h>                     // signal(), kill()
#include <sys/wait.h>                   // wait()
using namespace std;

void myhandler(int signum)
{
    cout << "子进程收到信号：" << signum << endl;
}

int main()
{
    pid_t pid = fork();

    if(pid == 0)                        // 子进程
    {
        signal(2, myhandler);
        cout << "子进程运行中，按 Ctrl+C 退出..." << endl;

        while(true)
        {
            sleep(1);                   // 保持运行。子进程休眠，等待父进程发送信号
        }
    }
    else if(pid > 0)                    // 父进程
    {
        sleep(3);                       // 父进程休眠，等待子进程运行
        kill(pid, 2);                   // 向子进程发送信号
        cout << "父进程向子进程发送信号 2" << endl;

        wait(NULL);                     // 等待子进程结束
        cout << "子进程已结束，父进程退出！" << endl;
    }
    else
    {
        perror("fork");                 // fork() 失败
    }

    return 0;
}
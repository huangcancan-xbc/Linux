#include <iostream>
#include <unistd.h>     // fork(), alarm(), sleep()
#include <signal.h>     // signal()
#include <sys/wait.h>   // wait()
using namespace std;

void alarm_handler(int sig)
{
    cout << "子进程收到 SIGALRM 信号，编号：" << sig << endl;
    exit(0);
}

int main()
{
    pid_t pid = fork();

    if (pid == 0)
    {
        cout << "子进程启动，PID: " << getpid() << endl;
        signal(SIGALRM, alarm_handler);

        alarm(5);                           // 设置 5 秒后触发 SIGALRM

        for (int i = 1; i <= 10; ++i)
        {
            cout << "子进程运行中: " << i << " 秒" << endl;
            sleep(1);
        }

        cout << "子进程正常结束。" << endl;
    }
    else if (pid > 0)
    {
        cout << "父进程启动，PID: " << getpid() << "，等待子进程..." << endl;

        int status;
        waitpid(pid, &status, 0);           // 阻塞等待子进程结束

        cout << "父进程检测到子进程结束，父进程退出。" << endl;
    }
    else
    {
        perror("fork");
    }

    return 0;
}
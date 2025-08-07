#include <iostream>
#include <unistd.h>     // fork(), sleep(), getpid(), exit()
#include <sys/wait.h>   // waitpid()
#include <cstdlib>      // exit()
using namespace std;

int main()
{
    pid_t id = fork();

    if (id == 0)
    {
        int cnt = 5;

        while (cnt)
        {
            cout << "我是子进程, 我的 pid 是: " << getpid() << ", 剩余次数: " << cnt << endl;

            sleep(1);               // 每秒打印一次
            cnt--;
        }

        // 注释掉下面这2行，子进程会正常退出，core dump（核心转储）也会显示 0，不注释则显示1
        int *p = nullptr;
        *p = 123;                   // 访问空指针，必触发 SIGSEGV，从而显示 core dump（核心转储）

        exit(0);                    // 子进程正常退出，返回码 0(永远走不到这里)
    }

    int status = 0;
    pid_t rid = waitpid(id, &status, 0);

    if (rid == id)
    {
        cout << "子进程结束，进程 id:" << rid << "   "<< "退出码:" << ((status >> 8) & 0xFF) << endl;
        cout << "退出信号: " << (status & 0x7F) << "   " << "核心转储: " << ((status >> 7) & 1) << endl;
    }

    return 0;
}
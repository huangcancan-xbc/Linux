#include <iostream>
#include <unistd.h>     // pipe, fork, write, close, read
#include <cstdlib>      // exit
#include <cstring>      // strlen
#include <sys/wait.h>   // waitpid
#include <cerrno>       // errno
#include <cstdio>       // perror
using namespace std;

// 子进程写入管道（触发SIGPIPE）
void ChildWrite(int wfd)
{
    const char* msg = "你好，父进程！\n";
    int count = 0;
    while (true)
    {
        cout << "子进程写入 #" << count++ << endl;
        ssize_t n = write(wfd, msg, strlen(msg));
        if (n == -1)
        {
            perror("子进程写入管道失败！");
            cout << "errno: " << errno << endl;
            break;
        }

        sleep(1);                           // 控制写入频率
    }

    // 正常情况下不会执行到这里（SIGPIPE会终止进程）
    cout << "子进程关闭写端，退出..." << endl;
    close(wfd);
    exit(1);
}

// 父进程读管道后关闭读端（触发子进程SIGPIPE）
void ParentRead(int rfd)
{
    char buf[1024];
    for (int i = 0; i < 5; ++i)
    {
        ssize_t n = read(rfd, buf, sizeof(buf) - 1);
        if (n > 0)
        {
            buf[n] = '\0';
            cout << "父进程读到：" << buf;
        }
        sleep(1);                           // 模拟读操作
    }

    cout << "父进程关闭读端（将触发子进程SIGPIPE）..." << endl;
    close(rfd);                             // 关闭读端
}

int main()
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("管道创建失败");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("创建子进程失败");
        return 2;
    }
    else if (pid == 0)                      // 子进程：关闭读端，持续写入
    {
        close(pipefd[0]);
        ChildWrite(pipefd[1]);
    }

    close(pipefd[1]);                       // 父进程：关闭写端，读取数据
    ParentRead(pipefd[0]);

    cout << "等待10秒，观察僵尸进程（PID=" << pid << "）..." << endl;
    sleep(10);                              // 延迟等待（此时子进程成为僵尸进程）

    // 回收子进程（将显示SIGPIPE终止）
    int status = 0;
    pid_t ret = waitpid(pid, &status, 0);
    if (ret > 0)
    {
        if (WIFSIGNALED(status))
        {
            int sig = WTERMSIG(status);
            cout << "子进程被信号终止，信号编号: " << sig << " (" << strsignal(sig) << ")" << endl;
        }
        else if (WIFEXITED(status))
        {
            cout << "子进程正常退出，exit code: " << WEXITSTATUS(status) << endl;
        }
        else
        {
            cout << "子进程异常退出" << endl;
        }
    }
    else
    {
        perror("waitpid 失败");
    }

    cout << "父进程退出..." << endl;
    return 0;
}
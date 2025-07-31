#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>                 // 提供 pipe, fork, read, write, sleep 等系统调用
#include <sys/types.h>
#include <sys/wait.h>               // 提供 waitpid
using namespace std;

#define N 2
#define NUM 1024

// 子进程写入函数
void Writer(int wfd)
{
    string s = "我是子进程";         // 待发送内容
    pid_t self = getpid();          // 获取当前子进程 PID
    int number = 0;

    char buffer[NUM];               // 缓冲区

    while (true)
    {
        sleep(1);                   // 每秒写一次，节省 CPU

        buffer[0] = 0;              // 清空缓冲区，表明我们是用它当字符串用

        // 构造发送信息：我是子进程-子进程pid-序号
        snprintf(buffer, sizeof(buffer), "%s-%d-%d", s.c_str(), self, number++);
        cout << buffer << endl;     // 本地输出（写给终端）

        // 将内容写入管道（系统调用 write），供父进程读取
        write(wfd, buffer, strlen(buffer));
    }
}

// 父进程读取函数
void Reader(int rfd)
{
    char buffer[NUM];               // 读取缓冲区

    while (true)
    {
        buffer[0] = 0;              // 清空

        // 读取管道数据，read 是系统调用
        ssize_t n = read(rfd, buffer, sizeof(buffer));

        if (n > 0)
        {
            buffer[n] = 0;          // 加上字符串结束符 '\0'，确保安全打印
            cout << "父进程收到消息[" << getpid() << "]# " << buffer << endl;
        }
        else if (n == 0)            // 返回 0 表示对端写入端关闭，读到 EOF
        {
            printf("父进程读到 EOF!\n");
            break;
        }
        else                        // 读取失败
        {
            break;
        }
    }
}

int main()
{
    int pipefd[N] = { 0 };       // pipefd[0]: read 端, pipefd[1]: write 端
    int n = pipe(pipefd);        // 创建匿名管道

    if (n < 0)
    {
        perror("pipe");
        return 1;                // 创建失败返回
    }

    // 创建子进程（fork）
    pid_t id = fork();
    if (id < 0)
    {
        perror("fork");
        return 2;                // 创建失败
    }

    if (id == 0)
    {
        // 子进程逻辑
        close(pipefd[0]);        // 关闭读端，只写

        Writer(pipefd[1]);       // 执行写入任务

        close(pipefd[1]);        // 写完关闭写端
        exit(0);                 // 退出子进程
    }

    // 父进程逻辑
    close(pipefd[1]);            // 父进程关闭写端，只读

    Reader(pipefd[0]);           // 执行读取任务（会阻塞等待）

    // 等待子进程退出
    pid_t rid = waitpid(id, 0, 0);
    if (rid < 0)
    {
        perror("waitpid");
        return 3;
    }

    close(pipefd[0]);            // 关闭读端

    sleep(5);                    // 给终端输出留个时间
    return 0;
}
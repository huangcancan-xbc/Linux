#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cerrno>
#include <sys/wait.h>
#include <pthread.h>
#include "Log.hpp"
#include "Task.hpp"
#include "ThreadPool.hpp"
using namespace std;

const string default_ip = "0.0.0.0";                               // 默认 IP 地址
const int default_port = 8080;                                     // 默认端口号
const int backlog = 10;                                            // 最大连接数
extern Log log;                                                    // 日志类

// 错误码枚举
enum
{
    UsageError = 1,   // 参数错误
    SocketError,      // 套接字创建失败
    BindError,        // 绑定失败
    ListenError       // 监听失败
};

// 线程数据结构体（用于 Run3）
class TCP_Server;     // 前向声明
struct ThreadData
{
    int client_fd;
    uint16_t port;
    string ip;
    TCP_Server* server_instance;                                   // 用于调用成员函数
};

class TCP_Server
{
private:
    int sockfd_;                                                   // socket 文件描述符
    string ip_;                                                    // IP 地址
    int port_;                                                     // 端口号
public:
    TCP_Server(string ip = default_ip, int port = default_port)    // 构造函数
        : ip_(ip), port_(port)
        {

        }

    ~TCP_Server()                                                  // 析构函数
    {

    }

    void Init()
    {
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd_ < 0)
        {
            log(Fatal, "创建 socket 失败，errno: %d, err string: %s", errno, strerror(errno));
            exit(SocketError);
        }
        log(Info, "创建 socket 成功，sockfd: %d", sockfd_);

        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port_);
        local.sin_addr.s_addr = inet_addr(ip_.c_str());

        if(bind(sockfd_, (struct sockaddr*)&local, sizeof(local)) < 0)
        {
            log(Fatal, "绑定 socket 失败，errno: %d, err string: %s", errno, strerror(errno));
            exit(BindError);
        }
        log(Info, "绑定 socket 成功，本地地址: %s:%d", ip_.c_str(), port_);

        if(listen(sockfd_, backlog) < 0)
        {
            log(Fatal, "监听 socket 失败，errno: %d, err string: %s", errno, strerror(errno));
            exit(ListenError);
        }
        log(Info, "监听 socket 成功，最大连接数: %d", backlog);
    }

    // 版本 1：单进程版本
    // 说明：每次只处理一个客户端连接，其它客户端需等待当前处理完成。
    // 缺点：并发能力差，不适用于高并发场景。
    void Run1()
    {
        for(;;)
        {
            struct sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);
            int client_fd = accept(sockfd_, (struct sockaddr*)&client_addr, &len);

            if(client_fd < 0)
            {
                log(Error, "接受客户端连接失败，errno: %d, err string: %s", errno, strerror(errno));
                continue;
            }

            uint16_t C_ = ntohs(client_addr.sin_port);
            char buf[32];
            inet_ntop(AF_INET, &(client_addr.sin_addr), buf, sizeof(buf));
            log(Info, "接收到客户端连接，sockfd: %d, 客户端 IP: %s, 客户端端口: %d", sockfd_,buf, C_);

            Serve(client_fd, C_, buf);
            close(client_fd);
        }
    }

    // 版本 2：多进程版本（使用孙子进程）
    // 说明：每当有新连接到来，就fork出一个子进程来处理。
    // 为了防止僵尸进程，采用了“孙子进程+父进程waitpid”的机制。
    // 孙子进程由init进程接管，从而避免产生僵尸进程。
    void Run2()
    {
        for(;;)
        {
            struct sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);
            int client_fd = accept(sockfd_, (struct sockaddr*)&client_addr, &len);

            if(client_fd < 0)
            {
                log(Error, "接受客户端连接失败，errno: %d, err string: %s", errno, strerror(errno));
                continue;
            }

            uint16_t C_ = ntohs(client_addr.sin_port);
            char buf[32];
            inet_ntop(AF_INET, &(client_addr.sin_addr), buf, sizeof(buf));
            log(Info, "接收到客户端连接，sockfd: %d, 客户端 IP: %s, 客户端端口: %d", sockfd_,buf, C_);

            pid_t id = fork(); // 第一次fork
            if (id == 0)                                            // 子进程
            {
                close(sockfd_);                                     // 关闭监听套接字

                if (fork() > 0)
                {
                    exit(0);                                        // 再次fork生成孙子进程，原子进程退出
                }

                // 此时孙子进程继承了sockfd，父进程退出后由init接管，防止僵尸进程
                Serve(client_fd, C_, buf);
                close(sockfd_);
                exit(0);                                            // 孙子进程执行完毕退出
            }

            close(client_fd);                                       // 父进程关闭连接套接字
            waitpid(id, nullptr, 0);                                // 回收子进程资源
        }

    }

    // 版本 3：多线程版本
    // 说明：每次收到连接请求，创建一个新线程来处理。
    // pthread_detach用于线程分离，使线程结束后自动释放资源。
    // 示例查看：ps -AL | grep -E "tcpserver|tcpclient"
    static void* Thread_Func(void* arg)
    {
        // pthread_detach(pthread_self());                             // 分离线程，防止资源泄漏
        ThreadData* data = static_cast<ThreadData*>(arg);
        data->server_instance->Serve(data->client_fd, data->port, const_cast<char*>(data->ip.c_str()));
        close(data->client_fd);
        delete data;  // 清理资源
        return nullptr;
    }

    void Run3()
    {
        for(;;)
        {
            struct sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);
            int client_fd = accept(sockfd_, (struct sockaddr*)&client_addr, &len);

            if(client_fd < 0)
            {
                log(Error, "接受客户端连接失败，errno: %d, err string: %s", errno, strerror(errno));
                continue;
            }

            uint16_t C_ = ntohs(client_addr.sin_port);
            char buf[32];
            inet_ntop(AF_INET, &(client_addr.sin_addr), buf, sizeof(buf));
            log(Info, "接收到客户端连接，sockfd: %d, 客户端 IP: %s, 客户端端口: %d", sockfd_,buf, C_);

            // 构造线程参数
            ThreadData* data = new ThreadData();
            data->client_fd = client_fd;
            data->port = C_;
            data->ip = string(buf);
            data->server_instance = this;

            pthread_t tid;
            if(pthread_create(&tid, nullptr, Thread_Func, data) != 0)  // 注意：直接传递 data
            {
                log(Error, "创建线程失败，errno: %d, err string: %s", errno, strerror(errno));
                delete data;
                close(client_fd);
                continue;
            }

            pthread_detach(tid);  // 分离线程        
        }
    }

    // 版本 4：线程池版本
    // 说明：通过预创建的线程池来处理连接请求。
    // 每次accept后将任务封装成Task对象推入线程池队列。
    // 优点是减少频繁创建销毁线程的开销，性能更好。
    // 示例查看：while true; do ps -AL | grep -E "tcpserver|tcpclient"; sleep 1; done
    void Run4()
    {
        ThreadPool<Task>::GetInstance()->Start();
        log(Info, "线程池启动成功");

        for(;;)
        {
            struct sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);
            int client_fd = accept(sockfd_, (struct sockaddr*)&client_addr, &len);

            if(client_fd < 0)
            {
                log(Error, "接受客户端连接失败，errno: %d, err string: %s", errno, strerror(errno));
                continue;
            }

            uint16_t C_ = ntohs(client_addr.sin_port);
            char buf[32];
            inet_ntop(AF_INET, &(client_addr.sin_addr), buf, sizeof(buf));
            log(Info, "接收到客户端连接，sockfd: %d, 客户端 IP: %s, 客户端端口: %d", sockfd_, buf, C_);

            // 使用 lambda 表达式封装处理逻辑
            Task task(client_fd, string(buf), C_, [this](int fd, uint16_t port, char* ip){
                        this->Serve(fd, port, ip);
                    });
            ThreadPool<Task>::GetInstance()->Push(task);
        }
    }

    void Serve(int client_fd, uint16_t port, char* ip)
    {
        char buf[4096];

        while(true)
        {
            ssize_t n = read(client_fd, buf, sizeof(buf));

            if(n > 0)
            {
                buf[n] = '\0';
                cout << "[客户端 " << ip << " : " << port << "]# " << buf << endl;

                string echo = "[服务器] 收到消息# ";
                echo += buf;    
                write(client_fd, echo.c_str(), echo.size());
            }
            else if(n == 0)
            {
                log(Info, "客户端断开连接，sockfd: %d, 客户端 IP: %s, 客户端端口: %d", client_fd, ip, port);
                break;
            }
            else
            {
                log(Error, "读取客户端数据失败，errno: %d, err string: %s", errno, strerror(errno));
                break;
            }
        }
    }
};
#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstring>
#include <functional>
#include <cstdlib>
#include "Log.hpp"
using namespace std;

// 错误码定义
enum
{
    SOCKET_ERR = 1,                                             // 创建 socket 失败
    BIND_ERR                                                    // bind 失败
};

// 默认端口和 IP 地址
static const uint16_t defaultport = 8080;
static const string defaultip = "0.0.0.0";

static const int BUFFER_SIZE = 1024;                            // 接收缓冲区大小
// 定义回调函数类型：接收一个字符串请求，返回一个字符串响应
typedef function<string(const string&)> func_t;

class UDP_Server
{
public:
    UDP_Server(const uint16_t &port = defaultport, const string &ip = defaultip,
              int enable_log = 1,
              int classification = 1,
              const string &log_path = "./log.txt",
              int console_out = 1)
        : sockfd_(0), port_(port), ip_(ip), isrunning_(false),
          log_(enable_log, classification, log_path, console_out) {}

    ~UDP_Server()
    {
        if(sockfd_ > 0)
        {
            close(sockfd_);                                     // 关闭 socket 文件描述符
            log_(Info, "UDP 服务器套接字已关闭");
        }
    }

    void Init()
    {
        // 1. 创建 UDP socket
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if(sockfd_ < 0)
        {
            log_(Fatal, "套接字创建错误，sockfd: %d", sockfd_);
            exit(SOCKET_ERR);
        }
        log_(Info, "套接字创建成功，sockfd: %d", sockfd_);

        // 2. 设置 IPv4 地址结构
        struct sockaddr_in local;
        bzero(&local, sizeof(local));                            // 内存清零
        local.sin_family = AF_INET;                              // IPv4
        local.sin_port = htons(port_);                           // 端口号转换为网络字节序
        local.sin_addr.s_addr = inet_addr(ip_.c_str());          // 将字符串 IP 转换为网络字节序整数

        // 3. 绑定 socket 到本地地址
        if(bind(sockfd_, (const struct sockaddr *)&local, sizeof(local)) < 0)
        {
            log_(Fatal, "绑定套接字错误，errno: %d，错误字符串: %s", errno, strerror(errno));
            exit(BIND_ERR);
        }
        log_(Info, "绑定套接字成功，本地地址: %s:%d", ip_.c_str(), port_);
    }

    void Run(func_t func)
    {
        isrunning_ = true;
        char buffer[BUFFER_SIZE];                               // 接收缓冲区

        while (isrunning_)
        {
            struct sockaddr_in client;                          // 客户端地址信息
            socklen_t len = sizeof(client);                     // 必须初始化长度

            // 接收客户端数据
            ssize_t n = recvfrom(sockfd_, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client, &len);
            if (n < 0)
            {
                log_(Warning, "recvfrom error, errno: %d, err string: %s", errno, strerror(errno));
                continue;
            }

            buffer[n] = '\0';                                   // 添加字符串结束符
            string request(buffer);                             // 构造请求字符串

            string response = func(request);                    // 调用用户自定义处理函数生成响应

            // 发送响应回客户端
            sendto(sockfd_, response.c_str(), response.size(), 0, (const struct sockaddr*)&client, len);
        }
    }

private:
    int sockfd_;         // socket 文件描述符
    string ip_;          // 服务器绑定的 IP 地址
    uint16_t port_;      // 服务器监听的端口号
    bool isrunning_;     // 服务运行状态标志
    Log log_;            // 日志对象成员
};
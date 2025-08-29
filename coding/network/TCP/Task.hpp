#pragma once

#include <string>
#include <functional>
using namespace std;

// 任务类：使用函数对象封装客户端连接处理任务
class Task
{
private:
    int client_fd_;                                    // 客户端文件描述符
    string client_ip_;                                 // 客户端IP地址
    uint16_t client_port_;                             // 客户端端口号
    function<void(int, uint16_t, char*)> handler_;     // 任务处理函数对象

public:
    Task(int fd = -1, const string& ip = "", uint16_t port = 0, 
         function<void(int, uint16_t, char*)> handler = nullptr)
        : client_fd_(fd), client_ip_(ip), client_port_(port), handler_(handler)
    {}

    // 用于设置任务信息
    void SetTask(int fd, const string& ip, uint16_t port, 
                 function<void(int, uint16_t, char*)> handler)
    {
        client_fd_ = fd;
        client_ip_ = ip;
        client_port_ = port;
        handler_ = handler;
    }

    // 重载函数调用操作符，使Task对象可以像函数一样被调用
    void operator()()
    {
        // 检查处理函数和文件描述符的有效性
        if (handler_ && client_fd_ >= 0)
        {
            // 调用处理函数处理客户端连接
            handler_(client_fd_, client_port_, const_cast<char*>(client_ip_.c_str()));
            // 关闭客户端连接
            close(client_fd_);
        }
    }

    // 用于获取任务执行结果
    string get_result() const
    {
        return "Task completed";
    }

    int get_client_fd() const { return client_fd_; }
    string get_client_ip() const { return client_ip_; }
    uint16_t get_client_port() const { return client_port_; }
};
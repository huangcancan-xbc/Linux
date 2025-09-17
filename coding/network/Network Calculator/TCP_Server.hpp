#pragma once

#include <functional>
#include <string>
#include <signal.h>
#include "Socket.hpp"
#include "Log.hpp"
// #include "Log.cpp"

using func_t = function<string(string &package)>;
const int data_buf_size = 4096;                                 // 接收客户端数据缓冲区大小

class TCP_Server
{
private:
    uint16_t port_;                                             // 监听端口号
    Sock listensock_;                                           // 监听套接字对象
    func_t callback_;                                           // 处理请求的回调函数

public:
    TCP_Server(uint16_t port, func_t callback)
        :port_(port), callback_(callback)
    {
        
    }

    ~TCP_Server()
    {
        listensock_.Close();
    }

    bool InitServer()
    {
        listensock_.Socket();                                   // 创建监听套接字
        listensock_.Bind(port_);                                // 绑定到指定端口
        listensock_.Listen();                                   // 开始监听连接
        log_(Info, "初始化服务器完成");                            // 日志记录初始化完成
        return true;
    }

    // void Run()
    // {
    //     signal(SIGCHLD, SIG_IGN);                               // 忽略子进程退出信号，防止僵尸进程
    //     signal(SIGPIPE, SIG_IGN);                               // 忽略管道信号，防止程序因写入已关闭的管道而崩溃

    //     while(true)
    //     {
    //         string client_IP;                                   // 存储客户端IP地址
    //         uint16_t client_port;                               // 存储客户端端口号
    //         int sockfd = listensock_.Accept(&client_IP, &client_port);      // 接受客户端连接

    //         if(sockfd < 0)
    //         {
    //             continue;                                       // 接受连接失败，继续循环
    //         }

    //         log_(Info, "新连接: %s:%d, fd=%d", client_IP.c_str(), client_port, sockfd);

    //         // 处理
    //         if(fork() == 0)                                     // 创建子进程处理客户端请求
    //         {
    //             listensock_.Close();                            // 关闭监听套接字，防止子进程重复接收连接
    //             string inbuf;                                   // 存储接收的数据流

    //             while(true)                                     // 循环处理客户端数据
    //             {
    //                 char buf[data_buf_size];                    // 接收缓冲区
    //                 ssize_t n = read(sockfd, buf, data_buf_size);   // 接受数据

    //                 if(n > 0)
    //                 {
    //                     buf[n] = '\0';                          // 确保字符串以'\0'结尾
    //                     inbuf += buf;                           // 追加到输入缓冲区

    //                     // 调试输出
    //                     // log_(Debug, "接收到数据: %s", buf);
    //                     // cout << "接收到数据: " << buf << endl;

    //                     // 处理请求
    //                     while(true)
    //                     {
    //                         string info = callback_(inbuf);     // 调用回调函数处理数据
    //                         if(info.empty())
    //                         {
    //                             break;
    //                         }

    //                         // 调试输出
    //                         // log_(Debug, "处理结果: %s", info.c_str());
    //                         // log_(Debug, "发送数据: %s", inbuf.c_str());
    //                         // cout << "处理结果: " << info << "数据流: " << inbuf << endl;

    //                         // 发送响应
    //                         write(sockfd, info.c_str(), info.size());
    //                     }
    //                 }
    //                 else if(n == 0)                             // 连接正常关闭
    //                 {
    //                     break;
    //                 }
    //                 else                                        // 出错
    //                 {
    //                     break;
    //                 }
    //             }
    //         }
    //     }
    // }

    void Run()
    {
        signal(SIGCHLD, SIG_IGN);
        signal(SIGPIPE, SIG_IGN);

        cout << "[DEBUG] 服务器开始运行，等待连接..." << endl;

        while(true)
        {
            string client_IP;
            uint16_t client_port;
            int sockfd = listensock_.Accept(&client_IP, &client_port);

            if(sockfd < 0)
            {
                continue;
            }

            cout << "[INFO] 新连接: " << client_IP << ":" << client_port << ", fd=" << sockfd << endl;
            log_(Info, "新连接: %s:%d, fd=%d", client_IP.c_str(), client_port, sockfd);

            if(fork() == 0)
            {
                listensock_.Close();
                string inbuf;

                while(true)
                {
                    char buf[data_buf_size];
                    ssize_t n = read(sockfd, buf, data_buf_size);

                    if(n > 0)
                    {
                        buf[n] = '\0';
                        inbuf += buf;
                        cout << "[DEBUG] 接收到数据: " << buf << endl;

                        // 处理粘包：持续处理完整的数据包
                        while(true)
                        {
                            string info = callback_(inbuf);
                            if(info.empty())
                            {
                                break;  // 没有完整数据包，等待更多数据
                            }
                            cout << "[DEBUG] 发送响应: " << info << endl;
                            write(sockfd, info.c_str(), info.size());
                        }
                    }
                    else if(n == 0)
                    {
                        cout << "[INFO] 客户端断开连接" << endl;
                        break;
                    }
                    else
                    {
                        cout << "[ERROR] 读取数据错误: " << strerror(errno) << endl;
                        break;
                    }
                }
                close(sockfd);
                exit(0);
            }
            close(sockfd);
        }
    }
};
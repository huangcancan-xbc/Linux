#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
#include "Socket.hpp"
#include "Log.hpp"


static const uint16_t default_port = 8082;      // 服务器默认监听端口
const int BUFFER_SIZE = 10240;                  // 缓冲区大小，用于接收 HTTP 请求
const std::string wwwroot = "./wwwroot";        // Web 根目录，所有静态资源都放在这个文件夹下
const std::string home_page = "index.html";     // 主页文件名
const std::string sep = "\r\n";                 // HTTP 请求行和头之间的分隔符



class Thread_Data
{
public:
    Thread_Data(int fd)                         // 构造函数：保存客户端 socket 文件描述符
    : sockfd(fd)
    {}          

public:
    int sockfd;                                 // 客户端连接的 socket fd
};





// HTTP 请求类
class HTTP_Request
{
public:
    std::vector<std::string> req_header;
    std::string text;

    // 解析结果
    std::string method;                         // 请求方法
    std::string url;                            // 请求 URL
    std::string version;                        // HTTP 版本
    std::string file_path;                      // 请求文件路径
public:
    // HTTP_Request()
    // {

    // }

    void Deserialize(std::string req)
    {
        while(true)
        {
            std::size_t pos = req.find(sep);    // 查找每行的结束位置（\r\n）
            if(pos == std::string::npos)        // 找不到
            {
                break;
            }

            std::string temp = req.substr(0, pos);          // 提取当前行
            if(temp.empty())
            {
                break;                          // 空行表示请求头结束
            }

            req_header.push_back(temp);
            req = req.substr(pos + 2);          // 去掉当前行和分隔符
        }

        text = req;                             // 剩余部分是请求正文，可能为空
    }

    void Parse()
    {
        std::stringstream ss(req_header[0]);    // 将请求行解析为字符串流
        ss >> method >> url >> version;         // 解析请求行

        file_path = wwwroot;                    // 默认请求文件路径为 wwwroot
        if(url == "/" || url == "index.html")   // 请求根目录或主页
        {
            file_path += ("/" + home_page);     // 文件路径为 wwwroot/index.html
        }
        else
        {
            file_path += url;                   // 请求文件路径为 wwwroot/url
        }
    }

    void DebugPrint()
    {
        for(auto &it : req_header)
        {
            std::cout << it << "\n\n";
        }

        std::cout << "method: " << method << std::endl;
        std::cout << "url: " << url << std::endl;
        std::cout << "version: " << version << std::endl;
        std::cout << "text: " << text << std::endl;
    }
};





// HTTP 服务器类
class HTTP_Server
{
private:
    Sock listensock_;                           // 监听 socket 封装对象
    uint16_t port_;                             // 服务器监听端口号
    std::unordered_map<std::string, std::string> content_type; // MIME 映射表

public:
    HTTP_Server(uint16_t port = default_port)   // 构造函数：初始化监听端口
        : port_(port)
    {

    }
    
    ~HTTP_Server()
    {

    }

    // 启动服务器，进入主循环：监听 -> 接收连接 -> 为连接分配线程处理
    void Start()
    {
        listensock_.Socket();                   // 创建监听 socket
        listensock_.Bind(port_);                // 绑定端口
        listensock_.Listen();                   // 开始监听

        for (;;)
        {
            std::string clientip;
            uint16_t clientport;

            // 接受一个新连接（阻塞等待）
            int sockfd = listensock_.Accept(&clientip, &clientport);
            if (sockfd < 0)
            {
                continue;                       // 失败则跳过，继续等待下一个连接
            }
            log_(Info, "get a new connect, sockfd: %d", sockfd);

            // 为每个连接分配一个线程进行处理
            pthread_t tid;
            Thread_Data* td = new Thread_Data(sockfd);
            pthread_create(&tid, nullptr, Thread_Run, td);
        }
    }

    // 处理 HTTP 请求：读取请求报文，构造并发送 HTTP 响应
    static void HandlerHttp1(int sockfd)
    {
        char buf[BUFFER_SIZE];
        ssize_t n = recv(sockfd, buf, BUFFER_SIZE - 1, 0);          // 读取请求数据
        if(n > 0)
        {
            buf[n] = '\0';                      // 添加字符串结束符
            std::cout << buf;

            std::string text ="Hello Linux!";   // 响应正文内容

            std::string response_line = "HTTP/1.1 200 OK\r\n";      // 状态行
            std::string response_header = "Content-Length: ";       // 响应头
            response_header += std::to_string(text.size());
            response_header += "\r\n";
            std::string blank_line = "\r\n";    // 空行

            std::string response = response_line;
            response += response_header;
            response += blank_line;
            response += text;

            send(sockfd, response.c_str(), response.size(), 0);     // 发送响应
        }
        close(sockfd);
    }



    static std::string Read_Htmlfile(const std::string &path)
    {
        std::ifstream in(path);
        if(!in.is_open())
        {
            return "404 Not Found";
        }

        std::string content;
        std::string line;
        while(std::getline(in, line))
        {
            content += line;
        }

        in.close();
        return content;
    }
    static void HandlerHttp2(int sockfd)
    {
        char buf[BUFFER_SIZE];
        ssize_t n = recv(sockfd, buf, BUFFER_SIZE - 1, 0);          // 读取请求数据
        if(n > 0)
        {
            buf[n] = '\0';                      // 添加字符串结束符
            std::cout << buf;

            std::string text = Read_Htmlfile("wwwroot/index.html"); // 读取并构造响应正文内容

            std::string response_line = "HTTP/1.1 200 OK\r\n";      // 状态行
            std::string response_header = "Content-Length: ";       // 响应头
            response_header += std::to_string(text.size());
            response_header += "\r\n";
            std::string blank_line = "\r\n";    // 空行

            std::string response = response_line;
            response += response_header;
            response += blank_line;
            response += text;

            send(sockfd, response.c_str(), response.size(), 0);     // 发送响应
        }
        close(sockfd);
    }


    static void HandlerHttp3(int sockfd)
    {
        char buf[BUFFER_SIZE];
        ssize_t n = recv(sockfd, buf, BUFFER_SIZE - 1, 0);          // 读取请求数据
        if(n > 0)
        {
            buf[n] = '\0';                      // 添加字符串结束符
            std::cout << buf;

            HTTP_Request req;
            req.Deserialize(buf);
            req.Parse();
            req.DebugPrint();

            std::string text = Read_Htmlfile(req.file_path);        // 读取并构造响应正文内容

            std::string response_line = "HTTP/1.1 200 OK\r\n";      // 状态行
            std::string response_header = "Content-Length: ";       // 响应头
            response_header += std::to_string(text.size());
            response_header += "\r\n";
            std::string blank_line = "\r\n";    // 空行

            std::string response = response_line;
            response += response_header;
            response += blank_line;
            response += text;

            send(sockfd, response.c_str(), response.size(), 0);     // 发送响应
        }
        close(sockfd);
    }


    static void HandlerHttp4(int sockfd)
    {
        char buf[BUFFER_SIZE];
        ssize_t n = recv(sockfd, buf, BUFFER_SIZE - 1, 0);          // 读取请求数据
        if(n > 0)
        {
            buf[n] = '\0';                      // 添加字符串结束符
            std::cout << buf;

            HTTP_Request req;
            req.Deserialize(buf);
            req.Parse();
            req.DebugPrint();

            bool flag = true;
            std::string text = Read_Htmlfile(req.file_path);        // 读取并构造响应正文内容
            if(text.empty())
            {
                flag = false;
                std::string err_thml = wwwroot + "/err.html";
                text = Read_Htmlfile(err_thml);
            }

            std::string response_line;          // 状态行
            if(flag)
            {
                response_line = "HTTP/1.1 200 OK\r\n";
            }
            else
            {
                response_line = "HTTP/1.1 404 Not Found\r\n";
            }

            std::string response_header = "Content-Length: ";       // 响应头
            response_header += std::to_string(text.size());

            // 示例：重定向（可取消注释使用）
            //response_header += "Location: https://minbit.top\r\n";

            response_header += "\r\n";
            std::string blank_line = "\r\n";    // 空行

            std::string response = response_line;
            response += response_header;
            response += blank_line;
            response += text;

            send(sockfd, response.c_str(), response.size(), 0);     // 发送响应
        }
        close(sockfd);
    }


    // 线程运行函数：分离线程 -> 调用处理函数 -> 清理资源
    static void* Thread_Run(void* args)
    {
        pthread_detach(pthread_self());         // 分离线程，自动回收资源
        Thread_Data* td = static_cast<Thread_Data*>(args);

        // 调用服务器请求处理函数，这里可以选择HandlerHttp1、HandlerHttp2、HandlerHttp3、HandlerHttp4进行不同处理
        // 然而，这些方法并不能使网页上的图片进行显示，原因是图片的Content-Type类型并没有被设置，因此需要在 HTTP 响应头中添加 Content-Type 字段
        // 于是我创建了HTTP_Server_image.hpp对图片（二进制）的处理，并在HTTP_Server_image.cc中调用HTTP_Server_image.hpp的处理函数
        HandlerHttp4(td->sockfd);

        delete td;                              // 释放申请的 Thread_Data
        return nullptr;
    }
};
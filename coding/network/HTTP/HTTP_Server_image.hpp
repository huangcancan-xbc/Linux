// 在HTTP_Server.hpp中，对于图片（二进制）的处理，需要额外处理，因此增加了一个HTTP_Server_image.hpp文件，用于处理图片相关的HTTP请求。
// Content-Type是需要设置的，参考：https://www.runoob.com/http/http-content-type.html
#pragma once

#include <iostream>
#include <string>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unordered_map>
#include "Socket.hpp"
#include "Log.hpp"

const std::string wwwroot = "./wwwroot";
const std::string sep = "\r\n";
const std::string homepage = "index.html";
static const int defaultport = 8082;
class HTTP_Server;

class Thread_Data
{
public:
    Thread_Data(int fd, HTTP_Server* s) : sockfd(fd), svr(s) {}

public:
    int sockfd;                                             // 客户端连接的 socket fd
    HTTP_Server* svr;                                        // 指向当前服务器实例，用于调用处理函数
};

class HttpRequest
{
public:
    void Deserialize(std::string req)
    {
        while (true)
        {
            std::size_t pos = req.find(sep);                // 查找每行的结束位置（\r\n）
            if (pos == std::string::npos) break;            // 找不到更多行了

            std::string temp = req.substr(0, pos);          // 提取当前行
            if (temp.empty()) break;                        // 空行表示头部结束

            req_header.push_back(temp);                     // 存入请求头向量
            req.erase(0, pos + sep.size());                 // 删除已处理的部分（包括 \r\n）
        }
        
        text = req;                                         // 剩余部分是请求体（body），可能为空
    }

    void Parse()
    {
        std::stringstream ss(req_header[0]);                // 使用 stringstream 解析第一行（请求行）
        ss >> method >> url >> version;

        file_path = wwwroot;                                // 初始化文件路径为根目录
        if (url == "/" || url == "/index.html")             // 如果请求的是根路径或首页，则返回 index.html
        {
            file_path += "/";
            file_path += homepage;
        }
        else
        {
            file_path += url;
        }

        // 提取文件后缀，用于设置 Content-Type
        auto pos = file_path.rfind(".");
        if (pos == std::string::npos)
        {
            suffix = ".html";                               // 默认为 html 类型
        }
        else
        {
            suffix = file_path.substr(pos);                 // 如 ".png", ".css"
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

public:
    std::vector<std::string> req_header;                   // 存储解析后的请求头部（每行一个字符串）
    std::string text;                                      // 请求体内容（如 POST 数据）

    std::string method;         // 请求方法：GET、POST 等
    std::string url;            // 请求的资源路径
    std::string version;        // 协议版本：HTTP/1.1
    std::string file_path;      // 对应的本地文件系统路径
    std::string suffix;         // 文件扩展名（含.），如 .html/.png
};

class HTTP_Server
{
public:
    HTTP_Server(uint16_t port = defaultport) : port_(port)
    {
        // 初始化 MIME 类型映射表（Content-Type）
        content_type.insert({ ".html", "text/html" });
        content_type.insert({ ".png", "image/png" });
        // 可继续扩展其他类型：.css, .js, .jpg 等
    }

    bool Start()
    {
        listensock_.Socket();           // 创建监听 socket
        listensock_.Bind(port_);        // 绑定端口
        listensock_.Listen();           // 开始监听

        for (;;)
        {
            std::string clientip;
            uint16_t clientport;

            // 接受一个新连接
            int sockfd = listensock_.Accept(&clientip, &clientport);
            if (sockfd < 0) continue;   // 失败则跳过

            log_(Info, "get a new connect, sockfd: %d", sockfd);

            pthread_t tid;
            Thread_Data* td = new Thread_Data(sockfd, this); // 传递 socket 和 server
            pthread_create(&tid, nullptr, Thread_Run, td);   // 创建线程执行 ThreadRun
        }
    }

    static std::string ReadHtmlContent(const std::string& htmlpath)
    {
        // 以二进制模式打开文件，防止文本模式干扰（如换行转换）
        std::ifstream in(htmlpath, std::ios::binary);
        if (!in.is_open())
        {
            return "";
        }

        // 获取文件大小
        in.seekg(0, std::ios_base::end);
        auto len = in.tellg();
        in.seekg(0, std::ios_base::beg);

        // 预分配内存
        std::string content;
        content.resize(len);

        // 一次性读取全部内容
        in.read((char*)content.c_str(), content.size());

        in.close();
        return content;
    }

    std::string SuffixToDesc(const std::string& suffix)
    {
        auto iter = content_type.find(suffix);
        if (iter == content_type.end())
        {
            return content_type[".html"]; // 默认返回 text/html
        }
        else
        {
            return content_type[suffix];
        }
    }

    void HandlerHttp(int sockfd)
    {
        char buffer[10240]; // 缓冲区接收请求数据

        // 接收客户端发送的数据
        ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0)
        {
            buffer[n] = '\0'; // 添加字符串结束符
            std::cout << "Received Request:\n" << buffer << std::endl;

            HttpRequest req;
            req.Deserialize(buffer);    // 拆分头部和体
            req.Parse();                // 解析请求行和路径

            std::string text;           // 响应正文内容
            bool flag = true;

            // 读取对应文件内容
            text = ReadHtmlContent(req.file_path);
            if (text.empty())
            {
                // 文件不存在，返回 404 页面
                flag = false;
                std::string err_html = wwwroot;
                err_html += "/";
                err_html += "err.html";
                text = ReadHtmlContent(err_html);
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
        pthread_detach(pthread_self());        // 分离线程，自动回收资源
        Thread_Data* td = static_cast<Thread_Data*>(args);
        td->svr->HandlerHttp(td->sockfd);      // 调用服务器处理函数
        delete td;                             // 释放申请的 Thread_Data
        return nullptr;
    }

    ~HTTP_Server()
    {

    }

private:
    Sock listensock_;                           // 监听 socket 封装对象
    uint16_t port_;                             // 服务器监听端口
    std::unordered_map<std::string, std::string> content_type; // Content-Type 映射表
};
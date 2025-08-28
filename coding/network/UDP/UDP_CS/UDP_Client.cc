#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

const int BUFSIZE = 1024;                                  // 接收服务器响应的缓冲区大小

void Usage(string proc)
{
    cout << "\n\r用法: " << proc << " 服务器IP 服务器端口号\n" << "例如: " << proc << " 127.0.0.1 8080\n" << endl;
}

int main(int argc, char* argv[])
{
    // 检查命令行参数数量，argc != 3 的原因：
    // argv[0]: 程序名称（如 "./udpclient"），argv[1]: 服务器IP地址（如 "127.0.0.1"），argv[2]: 服务器端口号（如 "8080"）
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(0);
    }

    // 1. 解析命令行参数获取服务器IP和端口
    string serverip = argv[1];                              // 获取服务器IP地址
    uint16_t serverport = stoi(argv[2]);                    // 将端口号字符串转换为整数

    // 2. 构造服务器地址结构
    struct sockaddr_in server;
    bzero(&server, sizeof(server));                         // 内存清零
    server.sin_family = AF_INET;                            // 设置地址族为IPv4
    server.sin_port = htons(serverport);                    // 端口号转换为网络字节序
    server.sin_addr.s_addr = inet_addr(serverip.c_str());   // IP地址转换为网络字节序

    // 3. 创建套接字
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);             // 创建UDP套接字
    if(sockfd < 0)
    {
        cout << "创建套接字失败！" << endl;
        exit(1);
    }
    //cout << "创建套接字成功！" << endl;

    // 客户端通常不需要显式调用bind函数！
    // 原因：
    // 1. 一个端口号只能被一个进程bind，对server是如此，对于client也是如此
    // 2. 客户端的端口号其实不重要，只要能保证主机上的唯一性就可以
    // 3. 系统会在首次发送数据时自动为客户端分配一个随机端口号（隐式bind）
    // 4. 这样可以避免端口冲突，简化客户端编程

    socklen_t server_len = sizeof(server);                   // 获取地址结构大小，用于后续的sendto和recvfrom函数
    string message;                                          // 存储用户输入的消息
    char buffer[BUFSIZE];                                    // 接收服务器响应的缓冲区

    // 4. 循环发送消息并接收服务器响应
    while(true)
    {
        cout << "请输入您要发送的消息@ ";
        getline(cin, message);                               // 读取用户输入的消息

        if(message == "q" || message == "exit" || message == "quit")
        {
            cout << "客户端退出！" << endl;
            break;
        }

        // 5. 发送消息
        // sendto参数说明：sockfd: socket文件描述符，message.c_str(): 要发送的数据，message.size(): 数据长度，
        // 0: 标志位（一般为0），(struct sockaddr *)&server: 目标地址结构，server_len: 地址结构大小
        ssize_t send_bytes = sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)& server, server_len);
        if(send_bytes < 0)
        {
            cout << "发送消息失败！" << endl;
            continue;
        }
        //cout << "发送消息成功！" << endl;

        // 6. 接收服务器响应
        struct sockaddr_in server_response;                   // 服务器响应的地址结构
        socklen_t server_response_len = sizeof(server_response);     // 服务器响应的地址结构大小

        // recvfrom参数说明：socket文件描述符，buffer: 接收缓冲区，缓冲区大小（留一个字节给'\0'），标志位（一般为0），发送方地址结构，地址结构大小（传入时是sizeof，返回时是实际大小）
        ssize_t recv_bytes = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)& server_response, &server_response_len);
        if (recv_bytes > 0)
        {
            buffer[recv_bytes] = '\0'; // 添加字符串结束符
            //cout << "收到服务器响应 (" << recv_bytes << " 字节): " << buffer << endl;
        }
        else if (recv_bytes == 0)
        {
            cout << "服务器关闭连接" << endl;
        }
        else
        {
            cout << "接收数据失败" << endl;
        }
        
    }

    // 7. 关闭套接字
    close(sockfd);
    cout << "客户端socket已关闭" << endl;

    return 0;
}
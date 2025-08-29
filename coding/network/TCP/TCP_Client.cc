#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
using namespace std;

void Usage(const string &proc)
{
    cout << "\n\r用法: " << proc << " IP 端口号\n" << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    string serverip = argv[1];
    uint16_t serverport = stoi(argv[2]);

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    inet_pton(AF_INET, serverip.c_str(), &(server.sin_addr));

    while (true)
    {
        int cnt = 5;
        int isreconnect = false;
        int sockfd = 0;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            cerr << "socket error" << endl;
            return 1;
        }

        do
        {
            int n = connect(sockfd, (struct sockaddr *)&server, sizeof(server));
            if (n < 0)
            {
                isreconnect = true;
                cnt--;
                cerr << "连接错误，重新连接： " << cnt << endl;
                sleep(2);
            }
            else
            {
                break;
            }
        } while (cnt && isreconnect);

        if (cnt == 0)
        {
            cerr << "用户离线" << endl;
            break;
        }

        while (true)
        {
            string message;
            cout << "请输入要发送的消息# ";
            getline(cin, message);

            int n = write(sockfd, message.c_str(), message.size());
            if (n < 0)
            {
                cerr << "写入错误！" << endl;
                break;
            }

            char inbuf[4096];
            n = read(sockfd, inbuf, sizeof(inbuf));
            if (n > 0)
            {
                inbuf[n] = 0;
                cout << inbuf << endl;
            }
            else
            {
                break;
            }
        }

        close(sockfd);
    }

    return 0;
}
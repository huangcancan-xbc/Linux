#include "TCP_Server.hpp"
#include "Protocol.hpp"
#include "Daemon.hpp"
#include "Server_Cal.hpp"

void Usage()
{
    cout << "\n\r用法:./TCP_Server 端口号[1024+]\n" << endl;
    cout << "示例：./TCP_Server 8080\n" << endl;
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage();
        exit(0);
    }
    
    uint16_t port = stoi(argv[1]);
    Server_Cal cal;
    TCP_Server *tsvp = new TCP_Server(port, bind(&Server_Cal::Calculator, &cal, placeholders::_1));
    tsvp->InitServer();
    // Daemon();
    // daemon(0, 0);   
    tsvp->Run();
    
    return 0;
}
#include "Udp_Server.hpp"
#include <memory>
#include <cstdio>

using namespace std;

void Usage(string proc)
{
    cout << "\n\rUsage: " << proc << " port[1024+]\n" << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }

    uint16_t port = stoi(argv[1]);

    unique_ptr<UdpServer> svr(new UdpServer(port));

    svr->Init();
    svr->Run();

    return 0;
}
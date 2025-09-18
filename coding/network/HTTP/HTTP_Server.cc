#include "HTTP_Server.hpp"
#include <iostream>
#include <memory>
#include <pthread.h>


using namespace std;

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        exit(1);
    }
    
    uint16_t port = std::stoi(argv[1]);
    HTTP_Server svr(port);
    svr.Start();
    return 0;

    // td::unique_ptr<HTTP_Server> svr(new HTTP_Server(port));
    // svr->Start();
    // return 0;
}

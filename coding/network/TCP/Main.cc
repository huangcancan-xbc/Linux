#include "TCP_Server.hpp"
#include <iostream>
#include <memory>

Log log(1, 0, "./log.txt", 1);

void Usage(string proc)
{
    cout << "\n\r用法: " << proc << " 端口号[1024+]\n" << endl;
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(UsageError);
    }

    uint16_t port = stoi(argv[1]);
    unique_ptr<TCP_Server> demo(new TCP_Server());
    demo->Init();
    demo->Run4();                   // 可选方法：Run1、Run2、Run3、Run4

    return 0;
}
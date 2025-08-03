#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <cerrno>
#include <cstdio>
using namespace std;

int main()
{
    key_t key = ftok("./tmp.txt", 65);  // 文件必须存在
    if (key == -1)
    {
        perror("ftok");
        return 1;
    }
    
    cout << "IPC Key: " << key << endl;
    return 0;
}
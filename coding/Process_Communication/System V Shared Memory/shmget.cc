#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <cerrno>
#include <cstdio>
using namespace std;

int main()
{
    key_t key = ftok("/tmp.txt", 65);                       // 生成唯一 key
    int shmid = shmget(key, 4096, 0666 | IPC_CREAT);        // 创建或获取 4KB 共享内存

    if (shmid == -1)
    {
        perror("shmget");
        return 1;
    }
    
    cout << "shmid: " << shmid << endl;
    return 0;
}
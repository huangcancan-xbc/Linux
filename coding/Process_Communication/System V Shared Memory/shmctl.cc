#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cerrno>
using namespace std;

int main()
{
    int key = ftok("/tmp.txt", 65);
    int shmid = shmget(key, 4096, 0666 | IPC_CREAT | IPC_EXCL);

    if(shmid == -1)
    {
        perror("shmget");
    }

    cout << "共享内存的ID为：" << shmid << endl;

    if(shmctl(shmid, IPC_RMID, nullptr))
    {
        perror("shmctl");
        return 1;
    }

    cout << "共享内存已经删除" << endl;

    return 0;
}
#pragma once

#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "Log.hpp"
using namespace std;

const char* pathname = "/home/hcc";
const int project_id = 0x1234;
const int shm_size = 4096;
#define FIFO_FILE "./myfifo"
#define MODE 0664
Log log(1, 0, "./log.txt", 1);

// 获取共享内存的key
key_t get_key()
{
    key_t key = ftok(pathname, project_id);

    if(key == -1)
    {
        cout << "共享内存的key创建失败！" << endl;
        exit(1);
    }

    return key;
}

// 创建共享内存
int create_shm(int flag)
{
    int shm_id = shmget(get_key(), shm_size, flag);

    if(shm_id == -1)
    {
        cout << "共享内存创建失败！" << endl;
        exit(1);
    }
    
    return shm_id;
}

// 只想创建共享内存，如果存在则报错
int create_shm_only()
{
    return create_shm(IPC_CREAT | IPC_EXCL | 0666);
}

// 获取共享内存，如果不存在则创建
int get_shm()
{
    return create_shm(IPC_CREAT | 0666);
}

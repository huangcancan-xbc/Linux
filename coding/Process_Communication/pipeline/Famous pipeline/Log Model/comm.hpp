#pragma once

#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "Log.hpp"
using namespace std;

#define FIFO_FILE "./myfifo"
#define MODE 0664

// enum
// {
//     FIFO_CREATE_ERR = 1,                    // 这是创建管道文件失败的错误码
//     FIFO_DELETE_ERR = 2,                    // 这是删除管道文件失败的错误码
//     FIFO_OPEN_ERR                           // 这是打开管道文件失败的错误码（枚举会自动赋值为3）
// };

class Init
{
public:
    Init()
    {
        int n = mkfifo(FIFO_FILE, MODE);    // 创建管道文件

        if (n == -1)
        {
            perror("mkfifo");
            exit(FIFO_CREATE_ERR);
        }
    }

    ~Init()
    {
        int m = unlink(FIFO_FILE);          // 删除管道文件
        
        if (m == -1)
        {
            perror("unlink");
            exit(FIFO_DELETE_ERR);
        }
    }
};
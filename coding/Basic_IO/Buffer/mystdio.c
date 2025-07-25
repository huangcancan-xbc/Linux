// 函数文件
#include "mystdio.h"
#include <sys/types.h>                                  // 基本系统数据类型定义 size_t
#include <sys/stat.h>                                   // 文件权限模式常量 S_IRWXU、S_IRWXG、S_IRWXO
#include <fcntl.h>                                      // 文件控制选项 O_CREAT、O_WRONLY、O_APPEND、O_RDONLY
#include <stdlib.h>                                     // malloc()、free()
#include <unistd.h>                                     // read()、write()、close()
#include <assert.h>                                     // assert() 断言检查

#define FILE_MODE 0666                                  // 默认文件权限 rw-rw-rw-

// 打开文件：支持 "w"（写入）、"a"（追加）、"r"（只读）
_FILE* _fopen(const char* filename, const char* flag)
{
    assert(filename);                                   // 确保文件名不为空
    assert(flag);                                       // 确保模式不为空

    int f = 0;                                          // 打开文件使用的标志位（flags）
    int fd = -1;                                        // 文件描述符初始化为非法值

    if (strcmp(flag, "w") == 0)
    {
        f = (O_CREAT | O_WRONLY | O_TRUNC);             // 创建+只写+截断旧内容
        fd = open(filename, f, FILE_MODE);
    }
    else if (strcmp(flag, "a") == 0)
    {
        f = (O_CREAT | O_WRONLY | O_APPEND);            // 创建+只写+追加写入
        fd = open(filename, f, FILE_MODE);
    }
    else if (strcmp(flag, "r") == 0)
    {
        f = O_RDONLY;                                   // 只读模式
        fd = open(filename, f);
    }
    else
    {
        return NULL;                                    // 不支持的模式，返回空指针
    }

    if (fd == -1)
    {
        return NULL;                                    // 打开失败
    }

    _FILE* fp = (_FILE*)malloc(sizeof(_FILE));          // 为 _FILE 结构体分配内存
    if (fp == NULL)
    {
        return NULL;                                    // 内存分配失败
    }

    fp->fileno = fd;                                    // 设置文件描述符
    //fp->flag = FLUSH_LINE;                            // 可选行刷新模式
    fp->flag = FLUSH_ALL;                               // 默认采用缓存满再写入
    fp->out_pos = 0;                                    // 输出缓冲区指针置 0

    return fp;
}

int _fwrite(_FILE* fp, const char* s, int len)          // 写入函数：将字符串写入到自定义缓冲区中
{
    // 将数据从字符串拷贝到输出缓冲区（假设缓冲足够）
    memcpy(&fp->outbuffer[fp->out_pos], s, len);        // 无边界检测（简化实现）
    fp->out_pos += len;

    if (fp->flag & FLUSH_NOW)                           // 判断刷新策略
    {
        write(fp->fileno, fp->outbuffer, fp->out_pos);  // 立即写入所有缓冲数据到文件
        fp->out_pos = 0;
    }
    else if (fp->flag & FLUSH_LINE)                     // 按行刷新
    {
        if (fp->outbuffer[fp->out_pos - 1] == '\n')     // 如果最后一个字符是换行符，则刷新
        {
            write(fp->fileno, fp->outbuffer, fp->out_pos);
            fp->out_pos = 0;
        }
    }
    else if (fp->flag & FLUSH_ALL)                      // 按缓存满刷新
    {
        if (fp->out_pos == SIZE)                        // 缓冲区满时刷新（一次写入 SIZE 字节）
        {
            write(fp->fileno, fp->outbuffer, fp->out_pos);
            fp->out_pos = 0;
        }
    }

    return len;                                         // 返回写入的长度
}

// 手动刷新函数（模拟 fflush）
void _fflush(_FILE* fp)
{
    if (fp->out_pos > 0)
    {
        write(fp->fileno, fp->outbuffer, fp->out_pos);  // 写入缓冲数据
        fp->out_pos = 0;
    }
}

// 关闭文件（释放资源）
void _fclose(_FILE* fp)
{
    if (fp == NULL) return;
    _fflush(fp);                                        // 关闭前确保缓冲区已写入
    close(fp->fileno);                                  // 关闭文件描述符
    free(fp);                                           // 释放分配的内存
}

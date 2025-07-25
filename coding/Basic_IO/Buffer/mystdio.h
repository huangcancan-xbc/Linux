// 头文件
//#pragma once                              // 可选，防止头文件重复包含
#ifndef __MYSTDIO_H__                       // 防止头文件重复包含（include guard）
#define __MYSTDIO_H__

#include <string.h>                         // 用于处理字符串函数，strcmp(), memcpy()

#define SIZE 1024                           // 缓冲区大小：1KB

// 缓冲刷新策略（flush strategy）标志
#define FLUSH_NOW 1                         // 每次写入立即刷新（立即写入文件）
#define FLUSH_LINE 2                        // 按行刷新（检测到换行符时刷新）
#define FLUSH_ALL 4                         // 缓冲区满了才刷新（默认）

// 自定义文件结构体，模拟 FILE 类型
typedef struct IO_FILE
{
    int fileno;                             // 文件描述符
    int flag;                               // 刷新策略
    //char inbuffer[SIZE];                  // 输入缓冲区（未实现）
    //int in_pos;                           // 输入缓冲位置指针
    char outbuffer[SIZE];                   // 输出缓冲区（output buffer）
    int out_pos;                            // 当前写入缓冲区的位置
}_FILE;

// 函数声明部分
_FILE* _fopen(const char* filename, const char* flag);          // 打开文件（模拟 fopen）
int _fwrite(_FILE* fp, const char* s, int len);                 // 写入字符串到缓冲区（模拟 fwrite）
void _fclose(_FILE* fp);                                        // 关闭文件（模拟 fclose）

#endif

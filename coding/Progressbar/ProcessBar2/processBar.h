#pragma once
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define NUM 102            			// 进度条数组的长度，定义进度条的最大容量
#define TOP 100            			// 最大进度，定义进度条的最大值（100%）
#define BODY '-'           			// 进度条主体符号，用于表示进度的主体部分
#define RIGHT '>'          			// 进度条尾部符号，用于表示当前进度的尾部
#define GREEN "\033[0;32;32m"  		// 设置绿色字体颜色
#define NONE "\033[m"          		// 恢复默认字体颜色

typedef void (*callback_t)(int);    // 定义函数指针类型，用于回调进度更新的函数
void processbar(int rate);
extern void download(callback_t cb);// 函数声明，模拟下载过程并调用回调函数更新进度
extern void initbar();              // 函数声明，用于初始化进度条

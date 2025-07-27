#pragma once						// 防止头文件被重复包含
#include <stdio.h>					// 引入标准库（虽然这个例子中不一定需要）

extern int myerrno;					// 声明一个全局变量，用于错误码（比如除0错误）

// 函数声明：提供加、减、乘、除功能

int sub(int x, int y);				// 减法
int mul(int x, int y);				// 乘法
int div(int x, int y);				// 除法（除数为0时返回-1，并设置myerrno=1）

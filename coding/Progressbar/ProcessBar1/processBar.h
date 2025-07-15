#pragma once
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define NUM 102         				   // 进度条数组长度, 代表进度条最大容量
#define TOP 100         				   // 最大进度, 定义进度条的最大进度（100%）
#define BODY '-'        				   // 进度条主体符号, 用于表示进度的主要部分
#define RIGHT '>'      					   // 进度条尾部符号, 用于显示进度条的结束位置

extern void processbar(int speed);  	   // 函数声明, 声明一个更新进度条的函数，speed为速度参数<Paste>

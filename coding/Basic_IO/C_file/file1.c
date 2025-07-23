#include <stdio.h>
#include <unistd.h>         // 包含系统调用的头文件，如getpid等
#include <string.h>         // 包含字符串处理函数的头文件，如strlen等

int main()
{
    FILE* fp = fopen("temp.txt", "w");

    if (fp == NULL)
    {
        perror("fopen");
        return 1;
    }

    return 0;
}
